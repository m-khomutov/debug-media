//
// Created by mkh on 16.11.2020.
//

#include "dmsps.h"

dm::h264::Sps::Sps( const uint8_t *data, size_t sz )
: m_bitreader( data, sz ),
  m_profile_idc( m_bitreader.byte() ),
  m_constraint_set0_flag( m_bitreader.bit() ),
  m_constraint_set1_flag( m_bitreader.bit() ),
  m_constraint_set2_flag( m_bitreader.bit() ),
  m_constraint_set3_flag( m_bitreader.bit() ),
  m_constraint_set4_flag( m_bitreader.bit() ),
  m_constraint_set5_flag( m_bitreader.bit() ),
  m_reserved_zero_2bits( m_bitreader.bits( 2 ) ),
  m_level_idc( m_bitreader.byte() ),
  m_seq_parameter_set_id( m_bitreader.uGolomb() ) {
        if( f_has_chroma_format_idc() ) {
        m_chroma_format_idc = m_bitreader.uGolomb();
        if( m_chroma_format_idc == 3 )
            m_separate_colour_plane_flag = m_bitreader.bit();
        m_bit_depth_luma_minus8 = m_bitreader.uGolomb();
        m_bit_depth_chroma_minus8 = m_bitreader.uGolomb();
        m_qpprime_y_zero_transform_bypass_flag = m_bitreader.bit();
        m_seq_scaling_matrix_present_flag = m_bitreader.bit();
        if( m_seq_scaling_matrix_present_flag ) {
            /*TODO ScalingList */
        }
    }
    if( !m_separate_colour_plane_flag )
        m_ChromaArrayType = m_chroma_format_idc;

    m_log2_max_frame_num_minus4 = m_bitreader.uGolomb();
    m_pic_order_cnt_type = m_bitreader.uGolomb();
    if( m_pic_order_cnt_type == 0 ) {
        m_log2_max_pic_order_cnt_lsb_minus4 = m_bitreader.uGolomb();
    }
   else if( m_pic_order_cnt_type == 1 ) {
        m_delta_pic_order_always_zero_flag = m_bitreader.bit();
        m_offset_for_non_ref_pic = m_bitreader.sGolomb();
        m_offset_for_top_to_bottom_field = m_bitreader.sGolomb();
        m_num_ref_frames_in_pic_order_cnt_cycle = m_bitreader.uGolomb();
        for( uint32_t i(0); i < m_num_ref_frames_in_pic_order_cnt_cycle; ++i )
            m_offset_for_ref_frame.push_back( m_bitreader.sGolomb() );
    }
    m_max_num_ref_frames = m_bitreader.uGolomb();
    m_gaps_in_frame_num_value_allowed_flag = m_bitreader.bit();
    m_pic_width_in_mbs_minus1 = m_bitreader.uGolomb();
    PicWidthInMbs = m_pic_width_in_mbs_minus1+1;
    PicWidthInSamples = PicWidthInMbs*16;
    m_pic_height_in_map_units_minus1 = m_bitreader.uGolomb();
    PicHeightInMapUnits = m_pic_height_in_map_units_minus1+1;
    PicSizeInMapUnits = PicWidthInMbs*PicHeightInMapUnits;
    m_frame_mbs_only_flag = m_bitreader.bit();
    FrameHeightInMbs=(2-m_frame_mbs_only_flag)*PicHeightInMapUnits;
    FrameHeightInSamples=FrameHeightInMbs*16;
    if( !m_frame_mbs_only_flag )
        m_mb_adaptive_frame_field_flag = m_bitreader.bit();
    m_direct_8x8_inference_flag = m_bitreader.bit();
    m_frame_cropping_flag = m_bitreader.bit();
    if( m_frame_cropping_flag ) {
        m_frame_crop_left_offset = m_bitreader.uGolomb();
        m_frame_crop_right_offset = m_bitreader.uGolomb();
        m_frame_crop_top_offset = m_bitreader.uGolomb();
        m_frame_crop_bottom_offset = m_bitreader.uGolomb();
    }
    m_vui_parameters_present_flag = m_bitreader.bit();
}

bool dm::h264::Sps::f_has_chroma_format_idc() {
  return m_profile_idc == 100 || m_profile_idc == 110 ||
         m_profile_idc == 122 || m_profile_idc == 244 || m_profile_idc == 44 ||
         m_profile_idc == 83 || m_profile_idc == 86 || m_profile_idc == 118 ||
         m_profile_idc == 128 || m_profile_idc == 138 || m_profile_idc == 139 ||
         m_profile_idc == 134 || m_profile_idc == 135;
}

