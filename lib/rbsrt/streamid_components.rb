# srt streamid parser based on https://github.com/Haivision/srt/blob/master/docs/AccessControl.md

require "uri"

module SRT
  class StreamIDComponents
    Types = ["stream", "file", "auth"]

    Modes = ["request", "publish", "bidirectional"]

    Header = "#!::"
    
    attr_accessor :resource_name
    attr_accessor :user_name
    attr_accessor :mode
    attr_accessor :host_name
    attr_accessor :type
    attr_accessor :sessionid
    attr_reader :extra

    def initialize(*args)

      @mode = :request
      @type = :stream
      @extra = {}

      # no args
      return unless args.first

      # hash args
      if args.first.class == ::Hash
        merge_hash!(args.first)
      elsif streamid = args.first.to_s
        if streamid.index(Header) == 0
          merge_str!(streamid)
        elsif streamid =~ URI::regexp
          merge_uri!(URI(streamid))
        else
          @resource_name = streamid
        end
      end
    end

    def to_s
      streamid = Header.dup

      vars = []

      vars << "r=#{resource_name}" if resource_name
      vars << "u=#{user_name}" if user_name
      vars << "m=#{mode}" if mode
      vars << "h=#{host_name}" if host_name
      vars << "t=#{type}" if type
      vars << "s=#{sessionid}" if sessionid
      
      extra.each { |key,val| vars << "#{key}=#{val}" }

      streamid + vars.join(",")
    end

    def method_missing(name, *args)
      return extra[name.to_s] if extra.key? name.to_s
      match = name.to_s.match(/^(.*?)(=?)$/)
      case match[2]
      when "="
        @extra[match[1]] = args.first
      else
        return nil
      end
    end

    def set(key, val)
      case key.to_s
      when 'resource_name', 'r' then @resource_name = val
      when 'user_name', 'u' then @user_name = val
      when 'mode', 'm' then @mode = val.to_sym if Modes.include?(val.to_s)
      when 'host_name', 'h' 
        if sep = val.index("/")
          @host_name = val[0..(sep - 1)]
          (@resource_name = val[(sep + 1)..-1] unless @resource_name)
        else
          @host_name = val
        end
      when 'type', 't' then (@type = val.to_sym if Types.include?(val.to_s))
      when 'sessionid', 's' then @sessionid = val
      else @extra[key] = val
      end
    end

    def merge_uri!(streamid)
      uri = URI(streamid)

      @host_name = uri.hostname
      @resource_name = uri.path[1..-1] if uri.path

      if uri.query && form = URI.decode_www_form(uri.query)
        merge_hash!(Hash[form])
      end
    end

    def merge_str!(streamid)
      streamid.scan(/([a-z0-9_-]+?)=(.+?)(,|$)/i).each do |key,val,_|
        set(key, val)
      end
    end

    def merge_hash!(hash)
      hash.each do |key, val|
        set(key, val)
      end
    end
  end
end