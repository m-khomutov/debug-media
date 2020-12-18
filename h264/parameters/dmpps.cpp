//
// Created by mkh on 15.12.2020.
//

#include <cmath>
#include "dmpps.h"

dm::h264::Pps::Pps( const uint8_t *data, size_t sz )
: m_bitreader( data+1, sz-1 ), /*first byte - nalu header*/
  m_pic_parameter_set_id( m_bitreader.uGolomb() ),
  m_seq_parameter_set_id( m_bitreader.uGolomb() ),
  m_entropy_coding_mode_flag( m_bitreader.bit() ),
  m_bottom_field_pic_order_in_frame_present_flag( m_bitreader.bit() ),
  m_num_slice_groups_minus1( m_bitreader.uGolomb() )
{
    if( m_num_slice_groups_minus1 > 0 ) {
        m_slice_group_map_type = m_bitreader.uGolomb();
        if( m_slice_group_map_type == 0 ) {
            for( int iGroup(0); iGroup <= m_num_slice_groups_minus1; ++iGroup )
                m_run_length_minus1.push_back( m_bitreader.uGolomb() );
        }
        else if( m_slice_group_map_type == 2 ) {
            for( int iGroup(0); iGroup < m_num_slice_groups_minus1; ++iGroup ) {
                m_top_left.push_back( m_bitreader.uGolomb() );
                m_bottom_right.push_back( m_bitreader.uGolomb() );
            }
        }
        else if( m_slice_group_map_type == 3 || m_slice_group_map_type == 4 || m_slice_group_map_type == 5 ) {
            m_slice_group_change_direction_flag = m_bitreader.bit();
            m_slice_group_change_rate_minus1 = m_bitreader.uGolomb();
        }
        else if( m_slice_group_map_type == 6 ) {
            m_pic_size_in_map_units_minus1 = m_bitreader.uGolomb();

            uint32_t b = ceil( log2( m_num_slice_groups_minus1 + 1 ) );
            for( int i(0); i <= m_pic_size_in_map_units_minus1; ++i )
                m_slice_group_id.push_back( m_bitreader.bits( b ) );
        }
    }
    m_num_ref_idx_l0_default_active_minus1 = m_bitreader.uGolomb();
    m_num_ref_idx_l1_default_active_minus1 = m_bitreader.uGolomb();
    m_weighted_pred_flag = m_bitreader.bit();
    m_weighted_bipred_idc = m_bitreader.bits( 2 );
    m_pic_init_qp_minus26 = m_bitreader.sGolomb();
    m_pic_init_qs_minus26 = m_bitreader.sGolomb();
    m_chroma_qp_index_offset = m_bitreader.sGolomb();
    m_deblocking_filter_control_present_flag = m_bitreader.bit();
    m_constrained_intra_pred_flag = m_bitreader.bit();
    m_redundant_pic_cnt_present_flag = m_bitreader.bit();
}

void dm::h264::Pps::f_read_scaling_list( int sizeOfScalingList, bool useDefaultScalingMatrixFlag ) {
    m_scaling_list.resize( sizeOfScalingList );

    int lastScale = 8;
    int nextScale = 8;
    for( int j = 0; j < sizeOfScalingList; j++ ) {
        if( nextScale != 0 ) {
            int delta_scale = m_bitreader.sGolomb();
            nextScale = ( lastScale + delta_scale + 256 ) % 256;
            useDefaultScalingMatrixFlag = ( j == 0 && nextScale == 0 );
        }
        m_scaling_list[ j ] = ( nextScale == 0 ) ? lastScale : nextScale;
        lastScale = m_scaling_list[ j ];
    }
}
