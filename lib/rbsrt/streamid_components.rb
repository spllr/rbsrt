# srt streamid parser based on https://github.com/Haivision/srt/blob/master/docs/AccessControl.md

module SRT
  class StreamIDComponents
    Types = ["stream", "file", "auth"]

    Modes = ["request", "publish", "bidirectional"]
    
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
        unless streamid.index("#!::") == 0
          @resource_name = streamid
        else
          merge_str!(streamid)
        end
      end
    end

    def to_s
      streamid = "!#::"

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

    def merge_str!(streamid)
      streamid.scan(/([a-z0-9_-]+?)=(.+?)(,|$)/i).each do |key,val,_|
        case key
        when "r" then @resource_name = val
        when "u" then @user_name = val
        when "m" then @mode = val.to_sym if Modes.include?(val)
        when "h" 
          if sep = val.index("/")
            @host_name = val[0..(sep - 1)]
            @resource_name = val[(sep + 1)..-1] unless @resource_name
          else
            @host_name = val
          end
        when "t" then @type = val.to_sym if Types.include?(val)
        when "s" then @sessionid = val
        else @extra[key] = val
        end
      end
    end

    def merge_hash!(hash)
      hash.each do |key, val|
        case key
        when :resource_name then @resource_name = val
        when :user_name then @user_name = val
        when :mode then @mode = val.to_sym if Modes.include?(val.to_s)
        when :host_name 
          if sep = val.index("/")
            @host_name = val[0..(sep - 1)]
            (@resource_name = val[(sep + 1)..-1] unless @resource_name)
          else
            @host_name = val
          end
        when :type then (@type = val.to_sym if Types.include?(val.to_s))
        when :sessionid then @sessionid = val
        else @extra[key] = val
      end
    end
    end
  end
end