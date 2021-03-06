/*
 * Ruby SRT - Ruby binding for Secure, Reliable, Transport
 * Copyright (c) 2019 Klaas Speller, Recce.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 */

/**
 * A Ruby wrapper for SRT (Secure Reliable Transport)
 * 
 * @author: Klaas Speller <klaas@recce.nl>
 */

#include <stdlib.h>

#include "rbsrt.h"
#include "rbstats.h"

// MARK: - Initializers

size_t rbsrt_stats_dsize(const void *stats)
{
  return sizeof(rbsrt_stats_t);
}

void rbsrt_stats_dmark(void *data)
{

}

void rbsrt_stats_deallocate(rbsrt_stats_t *stats)
{
    free(stats);
}

VALUE rbsrt_stat_allocate(VALUE klass)
{
    rbsrt_stats_t *stats = malloc(sizeof(rbsrt_stats_t));

    memset(stats, 0, sizeof(rbsrt_stats_t));
    
    return TypedData_Wrap_Struct(klass, &rbsrt_stats_rbtype, stats);
}

VALUE rbsrt_stats_initialize(int argc, VALUE* argv, VALUE self)
{
    VALUE arg1;
    VALUE opts;

    rb_scan_args(argc, argv, "1:", &arg1, &opts);

    RBSRT_SOCKET_BASE_UNWRAP(arg1, socket);
    
    RBSRT_STATS_UNWRAP(self, stats);

    int clear = NIL_P(opts) ? 0 : RTEST(rb_hash_aref(opts, RB_ID2SYM(rb_intern("clear"))));

    if (srt_bstats(socket->socket, &stats->perf, clear) == SRT_ERROR)
    {
        rbsrt_raise_last_srt_error();
    }

    return self;
}

// MARK: - Properties

VALUE rbsrt_stat_get_mstimestamp(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.msTimeStamp );
}

VALUE rbsrt_stat_get_pktsenttotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSentTotal );
}

VALUE rbsrt_stat_get_pktrecvtotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecvTotal );
}

VALUE rbsrt_stat_get_pktsndlosstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndLossTotal );
}

VALUE rbsrt_stat_get_pktrcvlosstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvLossTotal );
}

VALUE rbsrt_stat_get_pktretranstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRetransTotal );
}

VALUE rbsrt_stat_get_pktsentacktotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSentACKTotal );
}

VALUE rbsrt_stat_get_pktrecvacktotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecvACKTotal );
}

VALUE rbsrt_stat_get_pktsentnaktotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSentNAKTotal );
}

VALUE rbsrt_stat_get_pktrecvnaktotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecvNAKTotal );
}

VALUE rbsrt_stat_get_ussnddurationtotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.usSndDurationTotal );
}

VALUE rbsrt_stat_get_pktsnddroptotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndDropTotal );
}

VALUE rbsrt_stat_get_pktrcvdroptotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvDropTotal );
}

VALUE rbsrt_stat_get_pktrcvundecrypttotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvUndecryptTotal );
}

VALUE rbsrt_stat_get_pktsndfilterextratotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndFilterExtraTotal );
}

VALUE rbsrt_stat_get_pktrcvfilterextratotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterExtraTotal );
}

VALUE rbsrt_stat_get_pktrcvfiltersupplytotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterSupplyTotal );
}

VALUE rbsrt_stat_get_pktrcvfilterlosstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterLossTotal );
}

VALUE rbsrt_stat_get_bytesenttotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteSentTotal );
}

VALUE rbsrt_stat_get_byterecvtotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);
  
	return UINT2NUM( stats->perf.byteRecvTotal );
}

VALUE rbsrt_stat_get_bytercvlosstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvLossTotal );
}

VALUE rbsrt_stat_get_byteretranstotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRetransTotal );
}

VALUE rbsrt_stat_get_bytesnddroptotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteSndDropTotal );
}

VALUE rbsrt_stat_get_bytercvdroptotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvDropTotal );
}

VALUE rbsrt_stat_get_bytercvundecrypttotal(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvUndecryptTotal );
}

VALUE rbsrt_stat_get_pktsent(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSent );
}

VALUE rbsrt_stat_get_pktrecv(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecv );
}

VALUE rbsrt_stat_get_pktsndloss(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndLoss );
}

VALUE rbsrt_stat_get_pktrcvloss(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvLoss );
}

VALUE rbsrt_stat_get_pktretrans(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRetrans );
}

VALUE rbsrt_stat_get_pktrcvretrans(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvRetrans );
}

VALUE rbsrt_stat_get_pktsentack(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSentACK );
}

VALUE rbsrt_stat_get_pktrecvack(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecvACK );
}

VALUE rbsrt_stat_get_pktsentnak(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSentNAK );
}

VALUE rbsrt_stat_get_pktrecvnak(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRecvNAK );
}

VALUE rbsrt_stat_get_pktsndfilterextra(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndFilterExtra );
}

VALUE rbsrt_stat_get_pktrcvfilterextra(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterExtra );
}

VALUE rbsrt_stat_get_pktrcvfiltersupply(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterSupply );
}

VALUE rbsrt_stat_get_pktrcvfilterloss(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvFilterLoss );
}

VALUE rbsrt_stat_get_mbpssendrate(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.mbpsSendRate );
}

VALUE rbsrt_stat_get_mbpsrecvrate(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.mbpsRecvRate );
}

VALUE rbsrt_stat_get_ussndduration(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.usSndDuration );
}

VALUE rbsrt_stat_get_pktreorderdistance(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktReorderDistance );
}

VALUE rbsrt_stat_get_pktrcvavgbelatedtime(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.pktRcvAvgBelatedTime );
}

VALUE rbsrt_stat_get_pktrcvbelated(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvBelated );
}

VALUE rbsrt_stat_get_pktsnddrop(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndDrop );
}

VALUE rbsrt_stat_get_pktrcvdrop(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvDrop );
}

VALUE rbsrt_stat_get_pktrcvundecrypt(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvUndecrypt );
}

VALUE rbsrt_stat_get_bytesent(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteSent );
}

VALUE rbsrt_stat_get_byterecv(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRecv );
}

VALUE rbsrt_stat_get_bytercvloss(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvLoss );
}

VALUE rbsrt_stat_get_byteretrans(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRetrans );
}

VALUE rbsrt_stat_get_bytesnddrop(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteSndDrop );
}

VALUE rbsrt_stat_get_bytercvdrop(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvDrop );
}

VALUE rbsrt_stat_get_bytercvundecrypt(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return UINT2NUM( stats->perf.byteRcvUndecrypt );
}

VALUE rbsrt_stat_get_uspktsndperiod(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.usPktSndPeriod );
}

VALUE rbsrt_stat_get_pktflowwindow(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktFlowWindow );
}

VALUE rbsrt_stat_get_pktcongestionwindow(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktCongestionWindow );
}

VALUE rbsrt_stat_get_pktflightsize(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktFlightSize );
}

VALUE rbsrt_stat_get_msrtt(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.msRTT );
}

VALUE rbsrt_stat_get_mbpsbandwidth(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.mbpsBandwidth );
}

VALUE rbsrt_stat_get_byteavailsndbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.byteAvailSndBuf );
}

VALUE rbsrt_stat_get_byteavailrcvbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.byteAvailRcvBuf );
}

VALUE rbsrt_stat_get_mbpsmaxbw(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return LL2NUM( stats->perf.mbpsMaxBW );
}

VALUE rbsrt_stat_get_bytemss(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.byteMSS );
}

VALUE rbsrt_stat_get_pktsndbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktSndBuf );
}

VALUE rbsrt_stat_get_bytesndbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.byteSndBuf );
}

VALUE rbsrt_stat_get_mssndbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.msSndBuf );
}

VALUE rbsrt_stat_get_mssndtsbpddelay(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.msSndTsbPdDelay );
}

VALUE rbsrt_stat_get_pktrcvbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.pktRcvBuf );
}

VALUE rbsrt_stat_get_bytercvbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.byteRcvBuf );
}

VALUE rbsrt_stat_get_msrcvbuf(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.msRcvBuf );
}

VALUE rbsrt_stat_get_msrcvtsbpddelay(VALUE self)
{
	RBSRT_STATS_UNWRAP(self, stats);

	return INT2NUM( stats->perf.msRcvTsbPdDelay );
}

VALUE mSRTStatsKlass;

void RBSRT_stat_init(VALUE srt_module)
{
  mSRTStatsKlass = rb_define_class_under(srt_module, "Stats", rb_cObject);

  // Allocator

  rb_define_alloc_func(mSRTStatsKlass, rbsrt_stat_allocate);

  // Initializer
  
  rb_define_method(mSRTStatsKlass, "initialize", rbsrt_stats_initialize, -1);

  // time since the UDT entity is started, in milliseconds
  rb_define_method(mSRTStatsKlass, "msTimeStamp", rbsrt_stat_get_mstimestamp, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_time_stamp"), rb_intern("msTimeStamp"));
  rb_alias(mSRTStatsKlass, rb_intern("mstimestamp"), rb_intern("msTimeStamp"));

  // total number of sent data packets, including retransmissions
  rb_define_method(mSRTStatsKlass, "pktSentTotal", rbsrt_stat_get_pktsenttotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent_total"), rb_intern("pktSentTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsenttotal"), rb_intern("pktSentTotal"));

  // total number of received packets
  rb_define_method(mSRTStatsKlass, "pktRecvTotal", rbsrt_stat_get_pktrecvtotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv_total"), rb_intern("pktRecvTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecvtotal"), rb_intern("pktRecvTotal"));

  // total number of lost packets (sender side)
  rb_define_method(mSRTStatsKlass, "pktSndLossTotal", rbsrt_stat_get_pktsndlosstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_loss_total"), rb_intern("pktSndLossTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsndlosstotal"), rb_intern("pktSndLossTotal"));

  // total number of lost packets (receiver side)
  rb_define_method(mSRTStatsKlass, "pktRcvLossTotal", rbsrt_stat_get_pktrcvlosstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_loss_total"), rb_intern("pktRcvLossTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvlosstotal"), rb_intern("pktRcvLossTotal"));

  // total number of retransmitted packets
  rb_define_method(mSRTStatsKlass, "pktRetransTotal", rbsrt_stat_get_pktretranstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_retrans_total"), rb_intern("pktRetransTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktretranstotal"), rb_intern("pktRetransTotal"));

  // total number of sent ACK packets
  rb_define_method(mSRTStatsKlass, "pktSentACKTotal", rbsrt_stat_get_pktsentacktotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent_ack_total"), rb_intern("pktSentACKTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsentacktotal"), rb_intern("pktSentACKTotal"));

  // total number of received ACK packets
  rb_define_method(mSRTStatsKlass, "pktRecvACKTotal", rbsrt_stat_get_pktrecvacktotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv_ack_total"), rb_intern("pktRecvACKTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecvacktotal"), rb_intern("pktRecvACKTotal"));

  // total number of sent NAK packets
  rb_define_method(mSRTStatsKlass, "pktSentNAKTotal", rbsrt_stat_get_pktsentnaktotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent_nak_total"), rb_intern("pktSentNAKTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsentnaktotal"), rb_intern("pktSentNAKTotal"));

  // total number of received NAK packets
  rb_define_method(mSRTStatsKlass, "pktRecvNAKTotal", rbsrt_stat_get_pktrecvnaktotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv_nak_total"), rb_intern("pktRecvNAKTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecvnaktotal"), rb_intern("pktRecvNAKTotal"));

  // total time duration when UDT is sending data (idle time exclusive)
  rb_define_method(mSRTStatsKlass, "usSndDurationTotal", rbsrt_stat_get_ussnddurationtotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("us_snd_duration_total"), rb_intern("usSndDurationTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("ussnddurationtotal"), rb_intern("usSndDurationTotal"));

  // number of too-late-to-send dropped packets
  rb_define_method(mSRTStatsKlass, "pktSndDropTotal", rbsrt_stat_get_pktsnddroptotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_drop_total"), rb_intern("pktSndDropTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsnddroptotal"), rb_intern("pktSndDropTotal"));

  // number of too-late-to play missing packets
  rb_define_method(mSRTStatsKlass, "pktRcvDropTotal", rbsrt_stat_get_pktrcvdroptotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_drop_total"), rb_intern("pktRcvDropTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvdroptotal"), rb_intern("pktRcvDropTotal"));

  // number of undecrypted packets
  rb_define_method(mSRTStatsKlass, "pktRcvUndecryptTotal", rbsrt_stat_get_pktrcvundecrypttotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_undecrypt_total"), rb_intern("pktRcvUndecryptTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvundecrypttotal"), rb_intern("pktRcvUndecryptTotal"));

  // number of control packets supplied by packet filter
  rb_define_method(mSRTStatsKlass, "pktSndFilterExtraTotal", rbsrt_stat_get_pktsndfilterextratotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_filter_extra_total"), rb_intern("pktSndFilterExtraTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsndfilterextratotal"), rb_intern("pktSndFilterExtraTotal"));

  // number of control packets received and not supplied back
  rb_define_method(mSRTStatsKlass, "pktRcvFilterExtraTotal", rbsrt_stat_get_pktrcvfilterextratotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_extra_total"), rb_intern("pktRcvFilterExtraTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfilterextratotal"), rb_intern("pktRcvFilterExtraTotal"));

  // number of packets that the filter supplied extra (e.g. FEC rebuilt)
  rb_define_method(mSRTStatsKlass, "pktRcvFilterSupplyTotal", rbsrt_stat_get_pktrcvfiltersupplytotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_supply_total"), rb_intern("pktRcvFilterSupplyTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfiltersupplytotal"), rb_intern("pktRcvFilterSupplyTotal"));

  // number of packet loss not coverable by filter
  rb_define_method(mSRTStatsKlass, "pktRcvFilterLossTotal", rbsrt_stat_get_pktrcvfilterlosstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_loss_total"), rb_intern("pktRcvFilterLossTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfilterlosstotal"), rb_intern("pktRcvFilterLossTotal"));

  // total number of sent data bytes, including retransmissions
  rb_define_method(mSRTStatsKlass, "byteSentTotal", rbsrt_stat_get_bytesenttotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_sent_total"), rb_intern("byteSentTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("bytesenttotal"), rb_intern("byteSentTotal"));

  // total number of received bytes
  rb_define_method(mSRTStatsKlass, "byteRecvTotal", rbsrt_stat_get_byterecvtotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_recv_total"), rb_intern("byteRecvTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("byterecvtotal"), rb_intern("byteRecvTotal"));

  // total number of lost bytes SRT_ENABLE_LOSTBYTESCOUNT
  rb_define_method(mSRTStatsKlass, "byteRcvLossTotal", rbsrt_stat_get_bytercvlosstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_loss_total"), rb_intern("byteRcvLossTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvlosstotal"), rb_intern("byteRcvLossTotal"));

  // total number of retransmitted bytes
  rb_define_method(mSRTStatsKlass, "byteRetransTotal", rbsrt_stat_get_byteretranstotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_retrans_total"), rb_intern("byteRetransTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("byteretranstotal"), rb_intern("byteRetransTotal"));

  // number of too-late-to-send dropped bytes
  rb_define_method(mSRTStatsKlass, "byteSndDropTotal", rbsrt_stat_get_bytesnddroptotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_snd_drop_total"), rb_intern("byteSndDropTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("bytesnddroptotal"), rb_intern("byteSndDropTotal"));

  // number of too-late-to play missing bytes (estimate based on average packet size)
  rb_define_method(mSRTStatsKlass, "byteRcvDropTotal", rbsrt_stat_get_bytercvdroptotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_drop_total"), rb_intern("byteRcvDropTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvdroptotal"), rb_intern("byteRcvDropTotal"));

  // number of undecrypted bytes
  rb_define_method(mSRTStatsKlass, "byteRcvUndecryptTotal", rbsrt_stat_get_bytercvundecrypttotal, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_undecrypt_total"), rb_intern("byteRcvUndecryptTotal"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvundecrypttotal"), rb_intern("byteRcvUndecryptTotal"));

  // number of sent data packets, including retransmissions
  rb_define_method(mSRTStatsKlass, "pktSent", rbsrt_stat_get_pktsent, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent"), rb_intern("pktSent"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsent"), rb_intern("pktSent"));

  // number of received packets
  rb_define_method(mSRTStatsKlass, "pktRecv", rbsrt_stat_get_pktrecv, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv"), rb_intern("pktRecv"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecv"), rb_intern("pktRecv"));

  // number of lost packets (sender side)
  rb_define_method(mSRTStatsKlass, "pktSndLoss", rbsrt_stat_get_pktsndloss, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_loss"), rb_intern("pktSndLoss"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsndloss"), rb_intern("pktSndLoss"));

  // number of lost packets (receiver side)
  rb_define_method(mSRTStatsKlass, "pktRcvLoss", rbsrt_stat_get_pktrcvloss, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_loss"), rb_intern("pktRcvLoss"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvloss"), rb_intern("pktRcvLoss"));

  // number of retransmitted packets
  rb_define_method(mSRTStatsKlass, "pktRetrans", rbsrt_stat_get_pktretrans, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_retrans"), rb_intern("pktRetrans"));
  rb_alias(mSRTStatsKlass, rb_intern("pktretrans"), rb_intern("pktRetrans"));

  // number of retransmitted packets received
  rb_define_method(mSRTStatsKlass, "pktRcvRetrans", rbsrt_stat_get_pktrcvretrans, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_retrans"), rb_intern("pktRcvRetrans"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvretrans"), rb_intern("pktRcvRetrans"));

  // number of sent ACK packets
  rb_define_method(mSRTStatsKlass, "pktSentACK", rbsrt_stat_get_pktsentack, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent_ack"), rb_intern("pktSentACK"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsentack"), rb_intern("pktSentACK"));

  // number of received ACK packets
  rb_define_method(mSRTStatsKlass, "pktRecvACK", rbsrt_stat_get_pktrecvack, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv_ack"), rb_intern("pktRecvACK"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecvack"), rb_intern("pktRecvACK"));

  // number of sent NAK packets
  rb_define_method(mSRTStatsKlass, "pktSentNAK", rbsrt_stat_get_pktsentnak, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_sent_nak"), rb_intern("pktSentNAK"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsentnak"), rb_intern("pktSentNAK"));

  // number of received NAK packets
  rb_define_method(mSRTStatsKlass, "pktRecvNAK", rbsrt_stat_get_pktrecvnak, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_recv_nak"), rb_intern("pktRecvNAK"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrecvnak"), rb_intern("pktRecvNAK"));

  // number of control packets supplied by packet filter
  rb_define_method(mSRTStatsKlass, "pktSndFilterExtra", rbsrt_stat_get_pktsndfilterextra, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_filter_extra"), rb_intern("pktSndFilterExtra"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsndfilterextra"), rb_intern("pktSndFilterExtra"));

  // number of control packets received and not supplied back
  rb_define_method(mSRTStatsKlass, "pktRcvFilterExtra", rbsrt_stat_get_pktrcvfilterextra, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_extra"), rb_intern("pktRcvFilterExtra"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfilterextra"), rb_intern("pktRcvFilterExtra"));

  // number of packets that the filter supplied extra (e.g. FEC rebuilt)
  rb_define_method(mSRTStatsKlass, "pktRcvFilterSupply", rbsrt_stat_get_pktrcvfiltersupply, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_supply"), rb_intern("pktRcvFilterSupply"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfiltersupply"), rb_intern("pktRcvFilterSupply"));

  // number of packet loss not coverable by filter
  rb_define_method(mSRTStatsKlass, "pktRcvFilterLoss", rbsrt_stat_get_pktrcvfilterloss, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_filter_loss"), rb_intern("pktRcvFilterLoss"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvfilterloss"), rb_intern("pktRcvFilterLoss"));

  // sending rate in Mb/s
  rb_define_method(mSRTStatsKlass, "mbpsSendRate", rbsrt_stat_get_mbpssendrate, 0);
  rb_alias(mSRTStatsKlass, rb_intern("mbps_send_rate"), rb_intern("mbpsSendRate"));
  rb_alias(mSRTStatsKlass, rb_intern("mbpssendrate"), rb_intern("mbpsSendRate"));

  // receiving rate in Mb/s
  rb_define_method(mSRTStatsKlass, "mbpsRecvRate", rbsrt_stat_get_mbpsrecvrate, 0);
  rb_alias(mSRTStatsKlass, rb_intern("mbps_recv_rate"), rb_intern("mbpsRecvRate"));
  rb_alias(mSRTStatsKlass, rb_intern("mbpsrecvrate"), rb_intern("mbpsRecvRate"));

  // busy sending time (i.e., idle time exclusive)
  rb_define_method(mSRTStatsKlass, "usSndDuration", rbsrt_stat_get_ussndduration, 0);
  rb_alias(mSRTStatsKlass, rb_intern("us_snd_duration"), rb_intern("usSndDuration"));
  rb_alias(mSRTStatsKlass, rb_intern("ussndduration"), rb_intern("usSndDuration"));

  // size of order discrepancy in received sequences
  rb_define_method(mSRTStatsKlass, "pktReorderDistance", rbsrt_stat_get_pktreorderdistance, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_reorder_distance"), rb_intern("pktReorderDistance"));
  rb_alias(mSRTStatsKlass, rb_intern("pktreorderdistance"), rb_intern("pktReorderDistance"));

  // average time of packet delay for belated packets (packets with sequence past the ACK)
  rb_define_method(mSRTStatsKlass, "pktRcvAvgBelatedTime", rbsrt_stat_get_pktrcvavgbelatedtime, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_avg_belated_time"), rb_intern("pktRcvAvgBelatedTime"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvavgbelatedtime"), rb_intern("pktRcvAvgBelatedTime"));

  // number of received AND IGNORED packets due to having come too late
  rb_define_method(mSRTStatsKlass, "pktRcvBelated", rbsrt_stat_get_pktrcvbelated, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_belated"), rb_intern("pktRcvBelated"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvbelated"), rb_intern("pktRcvBelated"));

  // number of too-late-to-send dropped packets
  rb_define_method(mSRTStatsKlass, "pktSndDrop", rbsrt_stat_get_pktsnddrop, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_drop"), rb_intern("pktSndDrop"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsnddrop"), rb_intern("pktSndDrop"));

  // number of too-late-to play missing packets
  rb_define_method(mSRTStatsKlass, "pktRcvDrop", rbsrt_stat_get_pktrcvdrop, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_drop"), rb_intern("pktRcvDrop"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvdrop"), rb_intern("pktRcvDrop"));

  // number of undecrypted packets
  rb_define_method(mSRTStatsKlass, "pktRcvUndecrypt", rbsrt_stat_get_pktrcvundecrypt, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_undecrypt"), rb_intern("pktRcvUndecrypt"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvundecrypt"), rb_intern("pktRcvUndecrypt"));

  // number of sent data bytes, including retransmissions
  rb_define_method(mSRTStatsKlass, "byteSent", rbsrt_stat_get_bytesent, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_sent"), rb_intern("byteSent"));
  rb_alias(mSRTStatsKlass, rb_intern("bytesent"), rb_intern("byteSent"));

  // number of received bytes
  rb_define_method(mSRTStatsKlass, "byteRecv", rbsrt_stat_get_byterecv, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_recv"), rb_intern("byteRecv"));
  rb_alias(mSRTStatsKlass, rb_intern("byterecv"), rb_intern("byteRecv"));

  // number of retransmitted bytes SRT_ENABLE_LOSTBYTESCOUNT
  rb_define_method(mSRTStatsKlass, "byteRcvLoss", rbsrt_stat_get_bytercvloss, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_loss"), rb_intern("byteRcvLoss"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvloss"), rb_intern("byteRcvLoss"));

  // number of retransmitted bytes
  rb_define_method(mSRTStatsKlass, "byteRetrans", rbsrt_stat_get_byteretrans, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_retrans"), rb_intern("byteRetrans"));
  rb_alias(mSRTStatsKlass, rb_intern("byteretrans"), rb_intern("byteRetrans"));

  // number of too-late-to-send dropped bytes
  rb_define_method(mSRTStatsKlass, "byteSndDrop", rbsrt_stat_get_bytesnddrop, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_snd_drop"), rb_intern("byteSndDrop"));
  rb_alias(mSRTStatsKlass, rb_intern("bytesnddrop"), rb_intern("byteSndDrop"));

  // number of too-late-to play missing bytes (estimate based on average packet size)
  rb_define_method(mSRTStatsKlass, "byteRcvDrop", rbsrt_stat_get_bytercvdrop, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_drop"), rb_intern("byteRcvDrop"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvdrop"), rb_intern("byteRcvDrop"));

  // number of undecrypted bytes
  rb_define_method(mSRTStatsKlass, "byteRcvUndecrypt", rbsrt_stat_get_bytercvundecrypt, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_undecrypt"), rb_intern("byteRcvUndecrypt"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvundecrypt"), rb_intern("byteRcvUndecrypt"));

  // packet sending period, in microseconds
  rb_define_method(mSRTStatsKlass, "usPktSndPeriod", rbsrt_stat_get_uspktsndperiod, 0);
  rb_alias(mSRTStatsKlass, rb_intern("us_pkt_snd_period"), rb_intern("usPktSndPeriod"));
  rb_alias(mSRTStatsKlass, rb_intern("uspktsndperiod"), rb_intern("usPktSndPeriod"));

  // flow window size, in number of packets
  rb_define_method(mSRTStatsKlass, "pktFlowWindow", rbsrt_stat_get_pktflowwindow, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_flow_window"), rb_intern("pktFlowWindow"));
  rb_alias(mSRTStatsKlass, rb_intern("pktflowwindow"), rb_intern("pktFlowWindow"));

  // congestion window size, in number of packets
  rb_define_method(mSRTStatsKlass, "pktCongestionWindow", rbsrt_stat_get_pktcongestionwindow, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_congestion_window"), rb_intern("pktCongestionWindow"));
  rb_alias(mSRTStatsKlass, rb_intern("pktcongestionwindow"), rb_intern("pktCongestionWindow"));

  // number of packets on flight
  rb_define_method(mSRTStatsKlass, "pktFlightSize", rbsrt_stat_get_pktflightsize, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_flight_size"), rb_intern("pktFlightSize"));
  rb_alias(mSRTStatsKlass, rb_intern("pktflightsize"), rb_intern("pktFlightSize"));

  // RTT, in milliseconds
  rb_define_method(mSRTStatsKlass, "msRTT", rbsrt_stat_get_msrtt, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_rtt"), rb_intern("msRTT"));
  rb_alias(mSRTStatsKlass, rb_intern("msrtt"), rb_intern("msRTT"));

  // estimated bandwidth, in Mb/s
  rb_define_method(mSRTStatsKlass, "mbpsBandwidth", rbsrt_stat_get_mbpsbandwidth, 0);
  rb_alias(mSRTStatsKlass, rb_intern("mbps_bandwidth"), rb_intern("mbpsBandwidth"));
  rb_alias(mSRTStatsKlass, rb_intern("mbpsbandwidth"), rb_intern("mbpsBandwidth"));

  // available UDT sender buffer size
  rb_define_method(mSRTStatsKlass, "byteAvailSndBuf", rbsrt_stat_get_byteavailsndbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_avail_snd_buf"), rb_intern("byteAvailSndBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("byteavailsndbuf"), rb_intern("byteAvailSndBuf"));

  // available UDT receiver buffer size
  rb_define_method(mSRTStatsKlass, "byteAvailRcvBuf", rbsrt_stat_get_byteavailrcvbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_avail_rcv_buf"), rb_intern("byteAvailRcvBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("byteavailrcvbuf"), rb_intern("byteAvailRcvBuf"));

  // Transmit Bandwidth ceiling (Mbps)
  rb_define_method(mSRTStatsKlass, "mbpsMaxBW", rbsrt_stat_get_mbpsmaxbw, 0);
  rb_alias(mSRTStatsKlass, rb_intern("mbps_max_bw"), rb_intern("mbpsMaxBW"));
  rb_alias(mSRTStatsKlass, rb_intern("mbpsmaxbw"), rb_intern("mbpsMaxBW"));

  // MTU
  rb_define_method(mSRTStatsKlass, "byteMSS", rbsrt_stat_get_bytemss, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_mss"), rb_intern("byteMSS"));
  rb_alias(mSRTStatsKlass, rb_intern("bytemss"), rb_intern("byteMSS"));

  // UnACKed packets in UDT sender
  rb_define_method(mSRTStatsKlass, "pktSndBuf", rbsrt_stat_get_pktsndbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_snd_buf"), rb_intern("pktSndBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("pktsndbuf"), rb_intern("pktSndBuf"));

  // UnACKed bytes in UDT sender
  rb_define_method(mSRTStatsKlass, "byteSndBuf", rbsrt_stat_get_bytesndbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_snd_buf"), rb_intern("byteSndBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("bytesndbuf"), rb_intern("byteSndBuf"));

  // UnACKed timespan (msec) of UDT sender
  rb_define_method(mSRTStatsKlass, "msSndBuf", rbsrt_stat_get_mssndbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_snd_buf"), rb_intern("msSndBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("mssndbuf"), rb_intern("msSndBuf"));

  // Timestamp-based Packet Delivery Delay
  rb_define_method(mSRTStatsKlass, "msSndTsbPdDelay", rbsrt_stat_get_mssndtsbpddelay, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_snd_tsb_pd_delay"), rb_intern("msSndTsbPdDelay"));
  rb_alias(mSRTStatsKlass, rb_intern("mssndtsbpddelay"), rb_intern("msSndTsbPdDelay"));

  // Undelivered packets in UDT receiver
  rb_define_method(mSRTStatsKlass, "pktRcvBuf", rbsrt_stat_get_pktrcvbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("pkt_rcv_buf"), rb_intern("pktRcvBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("pktrcvbuf"), rb_intern("pktRcvBuf"));

  // Undelivered bytes of UDT receiver
  rb_define_method(mSRTStatsKlass, "byteRcvBuf", rbsrt_stat_get_bytercvbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("byte_rcv_buf"), rb_intern("byteRcvBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("bytercvbuf"), rb_intern("byteRcvBuf"));

  // Undelivered timespan (msec) of UDT receiver
  rb_define_method(mSRTStatsKlass, "msRcvBuf", rbsrt_stat_get_msrcvbuf, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_rcv_buf"), rb_intern("msRcvBuf"));
  rb_alias(mSRTStatsKlass, rb_intern("msrcvbuf"), rb_intern("msRcvBuf"));

  // Timestamp-based Packet Delivery Delay
  rb_define_method(mSRTStatsKlass, "msRcvTsbPdDelay", rbsrt_stat_get_msrcvtsbpddelay, 0);
  rb_alias(mSRTStatsKlass, rb_intern("ms_rcv_tsb_pd_delay"), rb_intern("msRcvTsbPdDelay"));
  rb_alias(mSRTStatsKlass, rb_intern("msrcvtsbpddelay"), rb_intern("msRcvTsbPdDelay"));
}
