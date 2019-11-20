require 'minitest/spec'

require "rbsrt"
require "thread"

describe SRT::Poll do
  it "can be created" do
    assert_instance_of(SRT::Poll, SRT::Poll.new)
  end

  

  describe "monitor server sockets" do
    before do
      # puts "server start"
      @server = SRT::Socket.new
      @server.bind "127.0.0.1", "6789"
      @server.listen 2
    end
  
    after do
      # puts "server stop"
      @client.close if @client
      @server.close if @server
    end

    it "will trigger when an new connection is available" do
      poll = SRT::Poll.new

      poll.add @server, :in

      Thread.new do
        # puts "connect to server"
        @client = SRT::Socket.new
        @client.connect "127.0.0.1", "6789"
      end


      # block/ yield style

      did_call = false

      poll.wait do |readable, writable, errors|  
        did_call = true
        refute_empty readable
        assert_empty writable
        assert_empty errors

        assert_equal @server, readable.first

        @accepted_sock = readable.first.accept

        poll.add @accepted_sock, :in, :err
      end

      assert_equal true, did_call, "wait block must yield"

      # return style

      Thread.new do
        # puts "write data to server"
        @client.sendmsg "data from client"
      end

      readable, writable, errors = poll.wait 1000

      refute_empty readable
      assert_empty writable
      assert_empty errors

      assert_equal @accepted_sock, readable.first

      assert_equal "data from client", readable.first.recvmsg

      # removing sockets

      poll.remove @client

      Thread.new do
        # puts "write data to server"
        @client.sendmsg "data from client"
      end

      readable, writable, errors = poll.wait 100

      assert_empty readable
      assert_empty writable
      assert_empty errors
    end
  end


  describe "adding and updating sockets" do
    before do
      # puts "server start"
      @server = SRT::Socket.new
      @server.bind "127.0.0.1", "6789"
      @server.listen 2
    end
  
    after do
      # puts "server stop"
      @client.close if @client
      @server.close if @server
    end

    it "can update a socket" do
      poll = SRT::Poll.new

      poll.add @server, :in

      Thread.new do
        @client = SRT::Socket.new
        @client.connect "127.0.0.1", "6789"
      end

      readable, _, _ = poll.wait 100

      remote_client = readable.first.accept

      # puts "++ IN OUT"
      poll.add remote_client, :err
      # puts "-- IN OUT"

      Thread.new do
        @client.write "foobar 1"
      end

      readable, writable, errors = poll.wait 100

      assert_empty readable
      assert_empty writable
      assert_empty errors

      _ = remote_client.read
      
      # upddate

      poll.update remote_client, :in

      Thread.new do
        @client.write "foobar 2"
      end

      readable, writable, errors = poll.wait 100

      # pending
      # refute_empty readable
      # assert_empty writable
      # assert_empty errors
    end
  end
end