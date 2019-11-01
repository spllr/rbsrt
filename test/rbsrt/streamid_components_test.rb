require 'minitest/spec'

require "rbsrt"

describe SRT::StreamIDComponents do
  it "can be created with no arguments" do
    parser = SRT::StreamIDComponents.new
    refute(parser == nil)
    parser.mode.must_equal :request
    parser.type.must_equal :stream
  end

  describe "parsing" do
    describe "non access control style streamid" do
      
      let(:parser) { 
        SRT::StreamIDComponents.new("mystream00")
      }

      it "uses the streamid as resource name" do
        parser.resource_name.must_equal "mystream00"
      end

      it "has no user name" do
        parser.user_name.must_be_nil
      end

      it "has :stream mode" do
        parser.mode.must_equal :request
      end

      it "has no host name" do
        parser.host_name.must_be_nil
      end

      it "has :request type" do
        parser.type.must_equal :stream
      end

      it "has no session id" do
        parser.sessionid.must_be_nil
      end

      it "has no additional info" do
        parser.extra.must_be_empty
      end
    end

    describe "access control style streamid string" do
      
      let(:streamid) { "#!::r=foobar,u=admin,h=stream.recce.nl,s=12345,t=stream,m=publish" }

      let(:parser) { 
        SRT::StreamIDComponents.new(streamid)
      }

      it "uses the 'r' var as the resource name" do
        parser.resource_name.must_equal "foobar"
      end

      it "uses the 'u' var as user name" do
        parser.user_name.must_equal "admin"
      end

      it "uses the 'm' var as mode" do
        parser.mode.must_equal :publish
      end

      it "uses the 'h' var as host name" do
        parser.host_name.must_equal "stream.recce.nl"
      end

      it "uses the 't' var as type" do
        parser.type.must_equal :stream
      end

      it "uses the 's' var as session id" do
        parser.sessionid.must_equal "12345"
      end
    end

    describe "type parsing" do
      it "parses 'stream' as :stream" do
        SRT::StreamIDComponents.new("#!::t=stream").type.must_equal :stream
      end

      it "parses 'file' as :file" do
        SRT::StreamIDComponents.new("#!::t=file").type.must_equal :file
      end

      it "parses 'auth' as :auth" do
        SRT::StreamIDComponents.new("#!::t=auth").type.must_equal :auth
      end

      it "parses unknown types to :stream" do
        SRT::StreamIDComponents.new("#!::t=foo").type.must_equal :stream
      end
    end

    describe "mode parsing" do
      it "parses 'request' as :request" do
        SRT::StreamIDComponents.new("#!::m=request").mode.must_equal :request
      end

      it "parses 'publish' as :publish" do
        SRT::StreamIDComponents.new("#!::m=publish").mode.must_equal :publish
      end

      it "parses 'bidirectional' as :bidirectional" do
        SRT::StreamIDComponents.new("#!::m=bidirectional").mode.must_equal :bidirectional
      end

      it "parses unknown modes to :request" do
        SRT::StreamIDComponents.new("#!::t=foo").mode.must_equal :request
      end
    end

    describe "host parsing" do
      it "removes any pathname from the host" do
        SRT::StreamIDComponents.new("#!::h=stream.recce.nl/my/movie.ts").host_name.must_equal "stream.recce.nl"
      end

      it "uses the host pathname as resource name when no 'r' var is provided" do
        SRT::StreamIDComponents.new("#!::h=stream.recce.nl/my/movie.ts").resource_name.must_equal "my/movie.ts"
      end

      it "does not use the host pathname as resource name when 'r' var is provided" do
        SRT::StreamIDComponents.new("#!::r=foo,h=stream.recce.nl/my/movie.ts").resource_name.must_equal "foo"
        SRT::StreamIDComponents.new("#!::h=stream.recce.nl/my/movie.ts,r=foo").resource_name.must_equal "foo"
      end
    end

    describe "parsing unknown values" do
      it "adds unknown values as properties" do
        parser = SRT::StreamIDComponents.new("#!::foo=bar")
        parser.extra.wont_be_empty
        parser.foo.must_equal "bar"
      end

      it "can add properties" do
        parser = SRT::StreamIDComponents.new
        
        parser.foo.must_be_nil nil

        parser.foo = "bar"
        parser.foo.must_equal "bar"
      end
    end

    describe "initialize with a hash" do

      let(:parser) do
        SRT::StreamIDComponents.new :mode => :publish, 
                                    :type => :stream, 
                                    :resource_name => "my_movie.ts"
      end

      it "sets the properties" do
        parser.mode.must_equal :publish
        parser.type.must_equal :stream
        parser.resource_name.must_equal "my_movie.ts"
      end
    end
  end

  describe "compiling" do

    let(:parser) { SRT::StreamIDComponents.new }
    
    before do
      parser.resource_name  = "myresource"
      parser.user_name      = "emma"
      parser.mode           = :publish
      parser.host_name      = "stream2.recce.nl"
      parser.type           = :file
      parser.sessionid      = "bcde"

      # extra properties 
      parser.baz            = "bar"
    end

    describe "to_s" do
      let(:streamid) { parser.to_s }

      it "can compile to a streamid string" do
        streamid.wont_be_nil
        streamid.must_be_instance_of String
        streamid.wont_be_empty
        streamid.must_match /^!#::/
      end

      it "sets the 'r' var to the resource name" do
        streamid.must_match /(::|,)r=myresource(,|$)/
      end

      it "sets the 'u' var to the user name" do
        streamid.must_match /(::|,)u=emma(,|$)/
      end

      it "sets the 'm' var to the mode" do
        streamid.must_match /(::|,)m=publish(,|$)/
      end

      it "sets the 'h' var to the host name" do
        streamid.must_match /(::|,)h=stream2.recce.nl(,|$)/
      end

      it "sets the 't' var to the type" do
        streamid.must_match /(::|,)t=file(,|$)/
      end

      it "sets the 's' var to the sessionid" do
        streamid.must_match /(::|,)s=bcde(,|$)/
      end

      it "adds extra attributes when available" do
        streamid.must_match /(::|,)baz=bar(,|$)/
      end
    end
  end
end
