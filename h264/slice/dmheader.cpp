//
// Created by mkh on 14.12.2020.
//

#include "dmheader.h"
#include "h264/dmnalu.h"
#include <cmath>
#include <iostream>

dm::h264::slice::Header::WeightList::WeightList( BitReader & bitreader, int chromaArrayType ) {
    m_luma_flag = bitreader.bit();
    if( m_luma_flag ) {
        m_luma.weight = bitreader.sGolomb();
        m_luma.offset = bitreader.sGolomb();
    }
    if( chromaArrayType != 0 ) {
        m_chroma_flag = bitreader.bit();
        if( m_chroma_flag ) {
            for( int i(0); i < 2; i++ ) {
                m_chroma[i].weight = bitreader.sGolomb();
                m_chroma[i].offset = bitreader.sGolomb();
            }
        }
    }
}

dm::h264::slice::Header::Header( const Sps * sps, const Pps * pps, const uint8_t *data, size_t sz )
: m_bitreader( data+1, sz-1 ), /*first byte - nalu header*/
  m_nal_unit_type( (data[0])&0x1F ),
  m_nal_ref_idc( ((data[0])>>5)&0x03 ),
  m_first_mb_in_slice( m_bitreader.uGolomb() ),
  m_slice_type( m_bitreader.uGolomb() ),
  m_idrPicFlag( ( m_nal_unit_type == h264::IDR ) ? true : false ),
  m_pic_parameter_set_id( m_bitreader.uGolomb() )
{
    if( m_slice_type > SliceType::SI )
        m_slice_type -= 5;

    if( sps->separateColourPlaneFlag() == 1 )
        m_colour_plane_id = m_bitreader.bits( 2 );

    m_frame_num = m_bitreader.bits( sps->maxFrameNum() );

    if( !sps->frameMbsOnlyFlag() ) {
        m_field_pic_flag = m_bitreader.bit();
        if( m_field_pic_flag )
            m_bottom_field_flag = m_bitreader.bit();
    }

    if( m_idrPicFlag )
        m_idr_pic_id = m_bitreader.uGolomb();

    if( sps->picOrderCountType() == 0 ) {
        m_pic_order_cnt_lsb = m_bitreader.bits( sps->maxPicOrderCountLsb() );
        if( pps->bottomFieldPicOrderInFramePresentFlag() && !m_field_pic_flag )
            m_delta_pic_order_cnt_bottom = m_bitreader.sGolomb();
    }
    if( sps->picOrderCountType() == 1 && !sps->deltaPicOrderAlwaysZeroFlag() ) {
        m_delta_pic_order_cnt[0] = m_bitreader.sGolomb();
        if( pps->bottomFieldPicOrderInFramePresentFlag() && !m_field_pic_flag )
            m_delta_pic_order_cnt[1] = m_bitreader.sGolomb();
    }
    if( pps->redundantPicCountPresentFlag() )
        m_redundant_pic_cnt = m_bitreader.uGolomb();
    if( m_slice_type == SliceType::B )
        m_direct_spatial_mv_pred_flag = m_bitreader.bit();
    if( m_slice_type == SliceType::P || m_slice_type == SliceType::SP || m_slice_type == SliceType::B ) {
        m_num_ref_idx_active_override_flag = m_bitreader.bit();
        if( m_num_ref_idx_active_override_flag ) {
            m_num_ref_idx_l0_active_minus1 = m_bitreader.uGolomb();
            if( m_slice_type == SliceType::B )
                m_num_ref_idx_l1_active_minus1 = m_bitreader.uGolomb();
        }
    }
    if( m_nal_unit_type == SliceExt || m_nal_unit_type == Slice3D ) {
        f_ref_pic_list_mvc_modification();
    }
    else
        f_ref_pic_list_modification();

    if( ( pps->weightedPredFlag() && ( m_slice_type == SliceType::P || m_slice_type == SliceType::SP ) ) ||
                                     ( pps->weightedBipredIdc() == 1 && m_slice_type == SliceType::B ) )
        f_pred_weight_table( sps );

    if( m_nal_ref_idc != 0 )
        f_dec_ref_pic_marking();

    if( pps->entropyCodingModeFlag() && m_slice_type != SliceType::I && m_slice_type != SliceType::SI )
        m_cabac_init_idc = m_bitreader.uGolomb();
    m_slice_qp_delta = m_bitreader.sGolomb();

    if( m_slice_type == SliceType::SP || m_slice_type == SliceType::SI ) {
        if( m_slice_type == SliceType::SP )
            m_sp_for_switch_flag = m_bitreader.bit();
        m_slice_qs_delta = m_bitreader.sGolomb();
    }

    if( pps->deblockingFilterControlPresentFlag() ) {
        m_disable_deblocking_filter_idc = m_bitreader.uGolomb();
        if( m_disable_deblocking_filter_idc != 1 ) {
            m_slice_alpha_c0_offset_div2 = m_bitreader.sGolomb();
            m_slice_beta_offset_div2 = m_bitreader.sGolomb();
        }
    }
    if( pps->numSliceGroupsMinus1() > 0 && pps->sliceGroupMapType() >= 3 && pps->sliceGroupMapType() <= 5 ) {
        uint32_t b = ceil( log2( pps->picSizeInMapUnits() / pps->sliceGroupChangeRate() + 1 ) );
        m_slice_group_change_cycle = m_bitreader.bits( b );
    }
    std::cerr << "type: " << m_slice_type << " num: " << m_frame_num << std::endl;
}

void dm::h264::slice::Header::f_ref_pic_list_mvc_modification() {
}

void dm::h264::slice::Header::f_ref_pic_list_modification() {
    if( m_slice_type % 5 != 2 && m_slice_type % 5 != 4 ) {
        m_ref_pic_list_modification_flag_l0 = m_bitreader.bit();
        if( m_ref_pic_list_modification_flag_l0 ) {
            int modification_of_pic_nums_idc;
            do {
                modification_of_pic_nums_idc = m_bitreader.uGolomb();
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 ) {
                    m_abs_diff_pic_num_minus1.reset( new int(m_bitreader.uGolomb()) );
                }
                else if( modification_of_pic_nums_idc == 2 )
                    m_long_term_pic_num.reset( new int(m_bitreader.uGolomb()) );
            }
            while( modification_of_pic_nums_idc != 3 );
        }
    }
    if( m_slice_type % 5 == 1 ) {
        m_ref_pic_list_modification_flag_l1 = m_bitreader.bit();
        if( m_ref_pic_list_modification_flag_l1 ) {
            int modification_of_pic_nums_idc;
            do {
                modification_of_pic_nums_idc = m_bitreader.uGolomb();
                if( modification_of_pic_nums_idc == 0 || modification_of_pic_nums_idc == 1 ) {
                    m_abs_diff_pic_num_minus1.reset( new int(m_bitreader.uGolomb()) );
                }
                else if( modification_of_pic_nums_idc == 2 )
                    m_long_term_pic_num.reset( new int(m_bitreader.uGolomb()) );
            }
            while( modification_of_pic_nums_idc != 3 );
        }
    }
}

void dm::h264::slice::Header::f_pred_weight_table( const Sps * sps ) {
    m_luma_log2_weight_denom = m_bitreader.uGolomb();
    if( sps->chromaArrayType() != 0 )
        m_chroma_log2_weight_denom = m_bitreader.uGolomb();

    for( int i(0); i <= m_num_ref_idx_l0_active_minus1; i++ )
        m_weight_l0.push_back( WeightList( m_bitreader, sps->chromaArrayType()) );

    if( m_slice_type % 5 == 1 ) {
        for( int i(0); i <= m_num_ref_idx_l1_active_minus1; i++ )
            m_weight_l1.push_back( WeightList( m_bitreader, sps->chromaArrayType()) );
    }
}

void dm::h264::slice::Header::f_dec_ref_pic_marking() {
    if( m_idrPicFlag ) {
        m_no_output_of_prior_pics_flag = m_bitreader.bit();
        m_long_term_reference_flag = m_bitreader.bit();
    }
    else {
        m_adaptive_ref_pic_marking_mode_flag = m_bitreader.bit();
        if( m_adaptive_ref_pic_marking_mode_flag ) {
            int memory_management_control_operation;
            do {
                memory_management_control_operation = m_bitreader.uGolomb();
                if( memory_management_control_operation == 1 || memory_management_control_operation == 3 ) {
                    m_difference_of_pic_nums_minus1.reset( new int(m_bitreader.uGolomb()) );
                }
                if( memory_management_control_operation == 2 ) {
                    m_long_term_pic_num.reset( new int(m_bitreader.uGolomb()) );
                }
                if( memory_management_control_operation == 3 || memory_management_control_operation == 6 ) {
                    m_long_term_frame_idx.reset( new int(m_bitreader.uGolomb()) );
                }
                if( memory_management_control_operation == 4 ) {
                    m_max_long_term_frame_idx_plus1.reset( new int(m_bitreader.uGolomb()) );
                }
            }
            while( memory_management_control_operation != 0 );
        }
    }
}