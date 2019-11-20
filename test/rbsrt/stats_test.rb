require 'minitest/spec'
require "rbsrt"

describe SRT::Stats do

  before do
    # puts "server start"
    @server = SRT::Socket.new
    @server.bind "127.0.0.1", "6789"
    @server.listen 2

    @client = SRT::Socket.new
    @client.connect "127.0.0.1", "6789"

    @remote_client = @server.accept
  end

  after do
    # puts "server stop"
    @remote_client.close if @remote_client
    @client.close if @client
    @server.close if @server
  end


  describe "properties and aliases" do
    let(:stats) do
      SRT::Stats.new @remote_client, :clear => false
    end

    it "has time since the UDT entity is started, in milliseconds" do
      assert_kind_of(Numeric, stats.msTimeStamp)
      assert_kind_of(Numeric, stats.ms_time_stamp)
      assert_kind_of(Numeric, stats.mstimestamp)
    end
    
    it "has total number of sent data packets, including retransmissions" do
      assert_kind_of(Numeric, stats.pktSentTotal)
      assert_kind_of(Numeric, stats.pkt_sent_total)
      assert_kind_of(Numeric, stats.pktsenttotal)
    end
    
    it "has total number of received packets" do
      assert_kind_of(Numeric, stats.pktRecvTotal)
      assert_kind_of(Numeric, stats.pkt_recv_total)
      assert_kind_of(Numeric, stats.pktrecvtotal)
    end
    
    it "has total number of lost packets (sender side)" do
      assert_kind_of(Numeric, stats.pktSndLossTotal)
      assert_kind_of(Numeric, stats.pkt_snd_loss_total)
      assert_kind_of(Numeric, stats.pktsndlosstotal)
    end
    
    it "has total number of lost packets (receiver side)" do
      assert_kind_of(Numeric, stats.pktRcvLossTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_loss_total)
      assert_kind_of(Numeric, stats.pktrcvlosstotal)
    end
    
    it "has total number of retransmitted packets" do
      assert_kind_of(Numeric, stats.pktRetransTotal)
      assert_kind_of(Numeric, stats.pkt_retrans_total)
      assert_kind_of(Numeric, stats.pktretranstotal)
    end
    
    it "has total number of sent ACK packets" do
      assert_kind_of(Numeric, stats.pktSentACKTotal)
      assert_kind_of(Numeric, stats.pkt_sent_ack_total)
      assert_kind_of(Numeric, stats.pktsentacktotal)
    end
    
    it "has total number of received ACK packets" do
      assert_kind_of(Numeric, stats.pktRecvACKTotal)
      assert_kind_of(Numeric, stats.pkt_recv_ack_total)
      assert_kind_of(Numeric, stats.pktrecvacktotal)
    end
    
    it "has total number of sent NAK packets" do
      assert_kind_of(Numeric, stats.pktSentNAKTotal)
      assert_kind_of(Numeric, stats.pkt_sent_nak_total)
      assert_kind_of(Numeric, stats.pktsentnaktotal)
    end
    
    it "has total number of received NAK packets" do
      assert_kind_of(Numeric, stats.pktRecvNAKTotal)
      assert_kind_of(Numeric, stats.pkt_recv_nak_total)
      assert_kind_of(Numeric, stats.pktrecvnaktotal)
    end
    
    it "has total time duration when UDT is sending data (idle time exclusive)" do
      assert_kind_of(Numeric, stats.usSndDurationTotal)
      assert_kind_of(Numeric, stats.us_snd_duration_total)
      assert_kind_of(Numeric, stats.ussnddurationtotal)
    end
    
    it "has number of too-late-to-send dropped packets" do
      assert_kind_of(Numeric, stats.pktSndDropTotal)
      assert_kind_of(Numeric, stats.pkt_snd_drop_total)
      assert_kind_of(Numeric, stats.pktsnddroptotal)
    end
    
    it "has number of too-late-to play missing packets" do
      assert_kind_of(Numeric, stats.pktRcvDropTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_drop_total)
      assert_kind_of(Numeric, stats.pktrcvdroptotal)
    end
    
    it "has number of undecrypted packets" do
      assert_kind_of(Numeric, stats.pktRcvUndecryptTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_undecrypt_total)
      assert_kind_of(Numeric, stats.pktrcvundecrypttotal)
    end
    
    it "has number of control packets supplied by packet filter" do
      assert_kind_of(Numeric, stats.pktSndFilterExtraTotal)
      assert_kind_of(Numeric, stats.pkt_snd_filter_extra_total)
      assert_kind_of(Numeric, stats.pktsndfilterextratotal)
    end
    
    it "has number of control packets received and not supplied back" do
      assert_kind_of(Numeric, stats.pktRcvFilterExtraTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_extra_total)
      assert_kind_of(Numeric, stats.pktrcvfilterextratotal)
    end
    
    it "has number of packets that the filter supplied extra (e.g. FEC rebuilt)" do
      assert_kind_of(Numeric, stats.pktRcvFilterSupplyTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_supply_total)
      assert_kind_of(Numeric, stats.pktrcvfiltersupplytotal)
    end
    
    it "has number of packet loss not coverable by filter" do
      assert_kind_of(Numeric, stats.pktRcvFilterLossTotal)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_loss_total)
      assert_kind_of(Numeric, stats.pktrcvfilterlosstotal)
    end
    
    it "has total number of sent data bytes, including retransmissions" do
      assert_kind_of(Numeric, stats.byteSentTotal)
      assert_kind_of(Numeric, stats.byte_sent_total)
      assert_kind_of(Numeric, stats.bytesenttotal)
    end
    
    it "has total number of received bytes" do
      assert_kind_of(Numeric, stats.byteRecvTotal)
      assert_kind_of(Numeric, stats.byte_recv_total)
      assert_kind_of(Numeric, stats.byterecvtotal)
    end
    
    it "has total number of lost bytes SRT_ENABLE_LOSTBYTESCOUNT" do
      assert_kind_of(Numeric, stats.byteRcvLossTotal)
      assert_kind_of(Numeric, stats.byte_rcv_loss_total)
      assert_kind_of(Numeric, stats.bytercvlosstotal)
    end
    
    it "has total number of retransmitted bytes" do
      assert_kind_of(Numeric, stats.byteRetransTotal)
      assert_kind_of(Numeric, stats.byte_retrans_total)
      assert_kind_of(Numeric, stats.byteretranstotal)
    end
    
    it "has number of too-late-to-send dropped bytes" do
      assert_kind_of(Numeric, stats.byteSndDropTotal)
      assert_kind_of(Numeric, stats.byte_snd_drop_total)
      assert_kind_of(Numeric, stats.bytesnddroptotal)
    end
    
    it "has number of too-late-to play missing bytes (estimate based on average packet size)" do
      assert_kind_of(Numeric, stats.byteRcvDropTotal)
      assert_kind_of(Numeric, stats.byte_rcv_drop_total)
      assert_kind_of(Numeric, stats.bytercvdroptotal)
    end
    
    it "has number of undecrypted bytes" do
      assert_kind_of(Numeric, stats.byteRcvUndecryptTotal)
      assert_kind_of(Numeric, stats.byte_rcv_undecrypt_total)
      assert_kind_of(Numeric, stats.bytercvundecrypttotal)
    end
    
    it "has number of sent data packets, including retransmissions" do
      assert_kind_of(Numeric, stats.pktSent)
      assert_kind_of(Numeric, stats.pkt_sent)
      assert_kind_of(Numeric, stats.pktsent)
    end
    
    it "has number of received packets" do
      assert_kind_of(Numeric, stats.pktRecv)
      assert_kind_of(Numeric, stats.pkt_recv)
      assert_kind_of(Numeric, stats.pktrecv)
    end
    
    it "has number of lost packets (sender side)" do
      assert_kind_of(Numeric, stats.pktSndLoss)
      assert_kind_of(Numeric, stats.pkt_snd_loss)
      assert_kind_of(Numeric, stats.pktsndloss)
    end
    
    it "has number of lost packets (receiver side)" do
      assert_kind_of(Numeric, stats.pktRcvLoss)
      assert_kind_of(Numeric, stats.pkt_rcv_loss)
      assert_kind_of(Numeric, stats.pktrcvloss)
    end
    
    it "has number of retransmitted packets" do
      assert_kind_of(Numeric, stats.pktRetrans)
      assert_kind_of(Numeric, stats.pkt_retrans)
      assert_kind_of(Numeric, stats.pktretrans)
    end
    
    it "has number of retransmitted packets received" do
      assert_kind_of(Numeric, stats.pktRcvRetrans)
      assert_kind_of(Numeric, stats.pkt_rcv_retrans)
      assert_kind_of(Numeric, stats.pktrcvretrans)
    end
    
    it "has number of sent ACK packets" do
      assert_kind_of(Numeric, stats.pktSentACK)
      assert_kind_of(Numeric, stats.pkt_sent_ack)
      assert_kind_of(Numeric, stats.pktsentack)
    end
    
    it "has number of received ACK packets" do
      assert_kind_of(Numeric, stats.pktRecvACK)
      assert_kind_of(Numeric, stats.pkt_recv_ack)
      assert_kind_of(Numeric, stats.pktrecvack)
    end
    
    it "has number of sent NAK packets" do
      assert_kind_of(Numeric, stats.pktSentNAK)
      assert_kind_of(Numeric, stats.pkt_sent_nak)
      assert_kind_of(Numeric, stats.pktsentnak)
    end
    
    it "has number of received NAK packets" do
      assert_kind_of(Numeric, stats.pktRecvNAK)
      assert_kind_of(Numeric, stats.pkt_recv_nak)
      assert_kind_of(Numeric, stats.pktrecvnak)
    end
    
    it "has number of control packets supplied by packet filter" do
      assert_kind_of(Numeric, stats.pktSndFilterExtra)
      assert_kind_of(Numeric, stats.pkt_snd_filter_extra)
      assert_kind_of(Numeric, stats.pktsndfilterextra)
    end
    
    it "has number of control packets received and not supplied back" do
      assert_kind_of(Numeric, stats.pktRcvFilterExtra)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_extra)
      assert_kind_of(Numeric, stats.pktrcvfilterextra)
    end
    
    it "has number of packets that the filter supplied extra (e.g. FEC rebuilt)" do
      assert_kind_of(Numeric, stats.pktRcvFilterSupply)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_supply)
      assert_kind_of(Numeric, stats.pktrcvfiltersupply)
    end
    
    it "has number of packet loss not coverable by filter" do
      assert_kind_of(Numeric, stats.pktRcvFilterLoss)
      assert_kind_of(Numeric, stats.pkt_rcv_filter_loss)
      assert_kind_of(Numeric, stats.pktrcvfilterloss)
    end
    
    it "has sending rate in Mb/s" do
      assert_kind_of(Numeric, stats.mbpsSendRate)
      assert_kind_of(Numeric, stats.mbps_send_rate)
      assert_kind_of(Numeric, stats.mbpssendrate)
    end
    
    it "has receiving rate in Mb/s" do
      assert_kind_of(Numeric, stats.mbpsRecvRate)
      assert_kind_of(Numeric, stats.mbps_recv_rate)
      assert_kind_of(Numeric, stats.mbpsrecvrate)
    end
    
    it "has busy sending time (i.e., idle time exclusive)" do
      assert_kind_of(Numeric, stats.usSndDuration)
      assert_kind_of(Numeric, stats.us_snd_duration)
      assert_kind_of(Numeric, stats.ussndduration)
    end
    
    it "has size of order discrepancy in received sequences" do
      assert_kind_of(Numeric, stats.pktReorderDistance)
      assert_kind_of(Numeric, stats.pkt_reorder_distance)
      assert_kind_of(Numeric, stats.pktreorderdistance)
    end
    
    it "has average time of packet delay for belated packets (packets with sequence past the ACK)" do
      assert_kind_of(Numeric, stats.pktRcvAvgBelatedTime)
      assert_kind_of(Numeric, stats.pkt_rcv_avg_belated_time)
      assert_kind_of(Numeric, stats.pktrcvavgbelatedtime)
    end
    
    it "has number of received AND IGNORED packets due to having come too late" do
      assert_kind_of(Numeric, stats.pktRcvBelated)
      assert_kind_of(Numeric, stats.pkt_rcv_belated)
      assert_kind_of(Numeric, stats.pktrcvbelated)
    end
    
    it "has number of too-late-to-send dropped packets" do
      assert_kind_of(Numeric, stats.pktSndDrop)
      assert_kind_of(Numeric, stats.pkt_snd_drop)
      assert_kind_of(Numeric, stats.pktsnddrop)
    end
    
    it "has number of too-late-to play missing packets" do
      assert_kind_of(Numeric, stats.pktRcvDrop)
      assert_kind_of(Numeric, stats.pkt_rcv_drop)
      assert_kind_of(Numeric, stats.pktrcvdrop)
    end
    
    it "has number of undecrypted packets" do
      assert_kind_of(Numeric, stats.pktRcvUndecrypt)
      assert_kind_of(Numeric, stats.pkt_rcv_undecrypt)
      assert_kind_of(Numeric, stats.pktrcvundecrypt)
    end
    
    it "has number of sent data bytes, including retransmissions" do
      assert_kind_of(Numeric, stats.byteSent)
      assert_kind_of(Numeric, stats.byte_sent)
      assert_kind_of(Numeric, stats.bytesent)
    end
    
    it "has number of received bytes" do
      assert_kind_of(Numeric, stats.byteRecv)
      assert_kind_of(Numeric, stats.byte_recv)
      assert_kind_of(Numeric, stats.byterecv)
    end
    
    it "has number of retransmitted bytes SRT_ENABLE_LOSTBYTESCOUNT" do
      assert_kind_of(Numeric, stats.byteRcvLoss)
      assert_kind_of(Numeric, stats.byte_rcv_loss)
      assert_kind_of(Numeric, stats.bytercvloss)
    end
    
    it "has number of retransmitted bytes" do
      assert_kind_of(Numeric, stats.byteRetrans)
      assert_kind_of(Numeric, stats.byte_retrans)
      assert_kind_of(Numeric, stats.byteretrans)
    end
    
    it "has number of too-late-to-send dropped bytes" do
      assert_kind_of(Numeric, stats.byteSndDrop)
      assert_kind_of(Numeric, stats.byte_snd_drop)
      assert_kind_of(Numeric, stats.bytesnddrop)
    end
    
    it "has number of too-late-to play missing bytes (estimate based on average packet size)" do
      assert_kind_of(Numeric, stats.byteRcvDrop)
      assert_kind_of(Numeric, stats.byte_rcv_drop)
      assert_kind_of(Numeric, stats.bytercvdrop)
    end
    
    it "has number of undecrypted bytes" do
      assert_kind_of(Numeric, stats.byteRcvUndecrypt)
      assert_kind_of(Numeric, stats.byte_rcv_undecrypt)
      assert_kind_of(Numeric, stats.bytercvundecrypt)
    end
    
    it "has packet sending period, in microseconds" do
      assert_kind_of(Numeric, stats.usPktSndPeriod)
      assert_kind_of(Numeric, stats.us_pkt_snd_period)
      assert_kind_of(Numeric, stats.uspktsndperiod)
    end
    
    it "has flow window size, in number of packets" do
      assert_kind_of(Numeric, stats.pktFlowWindow)
      assert_kind_of(Numeric, stats.pkt_flow_window)
      assert_kind_of(Numeric, stats.pktflowwindow)
    end
    
    it "has congestion window size, in number of packets" do
      assert_kind_of(Numeric, stats.pktCongestionWindow)
      assert_kind_of(Numeric, stats.pkt_congestion_window)
      assert_kind_of(Numeric, stats.pktcongestionwindow)
    end
    
    it "has number of packets on flight" do
      assert_kind_of(Numeric, stats.pktFlightSize)
      assert_kind_of(Numeric, stats.pkt_flight_size)
      assert_kind_of(Numeric, stats.pktflightsize)
    end
    
    it "has RTT, in milliseconds" do
      assert_kind_of(Numeric, stats.msRTT)
      assert_kind_of(Numeric, stats.ms_rtt)
      assert_kind_of(Numeric, stats.msrtt)
    end
    
    it "has estimated bandwidth, in Mb/s" do
      assert_kind_of(Numeric, stats.mbpsBandwidth)
      assert_kind_of(Numeric, stats.mbps_bandwidth)
      assert_kind_of(Numeric, stats.mbpsbandwidth)
    end
    
    it "has available UDT sender buffer size" do
      assert_kind_of(Numeric, stats.byteAvailSndBuf)
      assert_kind_of(Numeric, stats.byte_avail_snd_buf)
      assert_kind_of(Numeric, stats.byteavailsndbuf)
    end
    
    it "has available UDT receiver buffer size" do
      assert_kind_of(Numeric, stats.byteAvailRcvBuf)
      assert_kind_of(Numeric, stats.byte_avail_rcv_buf)
      assert_kind_of(Numeric, stats.byteavailrcvbuf)
    end
    
    it "has Transmit Bandwidth ceiling (Mbps)" do
      assert_kind_of(Numeric, stats.mbpsMaxBW)
      assert_kind_of(Numeric, stats.mbps_max_bw)
      assert_kind_of(Numeric, stats.mbpsmaxbw)
    end
    
    it "has MTU" do
      assert_kind_of(Numeric, stats.byteMSS)
      assert_kind_of(Numeric, stats.byte_mss)
      assert_kind_of(Numeric, stats.bytemss)
    end
    
    it "has UnACKed packets in UDT sender" do
      assert_kind_of(Numeric, stats.pktSndBuf)
      assert_kind_of(Numeric, stats.pkt_snd_buf)
      assert_kind_of(Numeric, stats.pktsndbuf)
    end
    
    it "has UnACKed bytes in UDT sender" do
      assert_kind_of(Numeric, stats.byteSndBuf)
      assert_kind_of(Numeric, stats.byte_snd_buf)
      assert_kind_of(Numeric, stats.bytesndbuf)
    end
    
    it "has UnACKed timespan (msec) of UDT sender" do
      assert_kind_of(Numeric, stats.msSndBuf)
      assert_kind_of(Numeric, stats.ms_snd_buf)
      assert_kind_of(Numeric, stats.mssndbuf)
    end
    
    it "has Timestamp-based Packet Delivery Delay" do
      assert_kind_of(Numeric, stats.msSndTsbPdDelay)
      assert_kind_of(Numeric, stats.ms_snd_tsb_pd_delay)
      assert_kind_of(Numeric, stats.mssndtsbpddelay)
    end
    
    it "has Undelivered packets in UDT receiver" do
      assert_kind_of(Numeric, stats.pktRcvBuf)
      assert_kind_of(Numeric, stats.pkt_rcv_buf)
      assert_kind_of(Numeric, stats.pktrcvbuf)
    end
    
    it "has Undelivered bytes of UDT receiver" do
      assert_kind_of(Numeric, stats.byteRcvBuf)
      assert_kind_of(Numeric, stats.byte_rcv_buf)
      assert_kind_of(Numeric, stats.bytercvbuf)
    end
    
    it "has Undelivered timespan (msec) of UDT receiver" do
      assert_kind_of(Numeric, stats.msRcvBuf)
      assert_kind_of(Numeric, stats.ms_rcv_buf)
      assert_kind_of(Numeric, stats.msrcvbuf)
    end
    
    it "has Timestamp-based Packet Delivery Delay" do
      assert_kind_of(Numeric, stats.msRcvTsbPdDelay)
      assert_kind_of(Numeric, stats.ms_rcv_tsb_pd_delay)
      assert_kind_of(Numeric, stats.msrcvtsbpddelay)
    end
  end

  describe "clearing vs non-clearing" do
    
    it "will not clear stats by default" do
      # initial data

      @client.write "foobar"
      _ = @remote_client.read

      stats = SRT::Stats.new(@client)
      byte_sent = stats.byte_sent
      assert(byte_sent > 0, "should have sent bytes")


      # same data
      
      stats2 = SRT::Stats.new(@client)
      byte_sent_2 = stats2.byte_sent
      assert_equal(byte_sent_2, byte_sent, "should have same bytes")

      # more data

      @client.write "baz"
      _ = @remote_client.read

      stats3 = SRT::Stats.new(@client)
      byte_sent_3 = stats3.byte_sent
      assert(byte_sent_3 > byte_sent, "should have more bytes")
    end

    it "will not clear stats when :clear => false" do
      # initial data

      @client.write "foobar"
      _ = @remote_client.read

      stats = SRT::Stats.new(@client, :clear => false)
      byte_sent = stats.byte_sent
      assert(byte_sent > 0, "should have sent bytes")


      # same data
      
      stats2 = SRT::Stats.new(@client, :clear => false)
      byte_sent_2 = stats2.byte_sent
      assert_equal(byte_sent_2, byte_sent, "should have same bytes")

      # more data

      @client.write "baz"
      _ = @remote_client.read

      stats3 = SRT::Stats.new(@client, :clear => false)
      byte_sent_3 = stats3.byte_sent
      assert(byte_sent_3 > byte_sent, "should have more bytes")
    end

    it "will clear stats when :clear => true" do
      # initial data

      @client.write "foobar"
      _ = @remote_client.read

      stats = SRT::Stats.new(@client, :clear => true)
      byte_sent = stats.byte_sent
      assert(byte_sent > 0, "should have sent bytes")

      # more data

      @client.write "baz"
      _ = @remote_client.read

      stats3 = SRT::Stats.new(@client, :clear => true)
      byte_sent_3 = stats3.byte_sent
      assert(byte_sent_3 < byte_sent, "should have less bytes")
    end
  end
end