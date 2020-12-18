//
// Created by mkh on 16.11.2020.
//
#pragma once

#include "h264/dmbitreader.h"

namespace dm {
    namespace h264 {
        class Sps {
        public:
            Sps( const uint8_t * data, size_t sz );

            bool separateColourPlaneFlag() const {
                return m_separate_colour_plane_flag;
            }
            uint32_t maxFrameNum() const {
                return m_log2_max_frame_num_minus4 + 4;
            }
            bool frameMbsOnlyFlag() const {
                return m_frame_mbs_only_flag;
            }
            uint32_t picOrderCountType() const {
                return m_pic_order_cnt_type;
            }
            uint32_t maxPicOrderCountLsb() const {
                return m_log2_max_pic_order_cnt_lsb_minus4 + 4;
            }
            bool deltaPicOrderAlwaysZeroFlag() const {
                return m_delta_pic_order_always_zero_flag;
            }
            int chromaArrayType() const {
                if( m_separate_colour_plane_flag )
                    return 0;
                return m_chroma_format_idc;
            }
            int width() const {
                return PicWidthInSamples;
            }
            int height() const {
                return FrameHeightInSamples;
            }

        private:
            BitReader m_bitreader;

            uint32_t m_profile_idc;
            uint32_t m_constraint_set0_flag;
            uint32_t m_constraint_set1_flag;
            uint32_t m_constraint_set2_flag;
            uint32_t m_constraint_set3_flag;
            uint32_t m_constraint_set4_flag;
            uint32_t m_constraint_set5_flag;
            uint32_t m_reserved_zero_2bits;
            uint32_t m_level_idc;
            uint32_t m_seq_parameter_set_id;
            uint32_t m_chroma_format_idc{1};
            bool m_separate_colour_plane_flag{false};
            uint32_t m_bit_depth_luma_minus8{0};
            uint32_t m_bit_depth_chroma_minus8{0};
            bool m_qpprime_y_zero_transform_bypass_flag{false};
            bool m_seq_scaling_matrix_present_flag{false};
            uint32_t m_log2_max_frame_num_minus4{0};
            uint32_t m_pic_order_cnt_type{0};
            uint32_t m_log2_max_pic_order_cnt_lsb_minus4{0};
            bool m_delta_pic_order_always_zero_flag{false};
            uint32_t m_offset_for_non_ref_pic{0};
            uint32_t m_offset_for_top_to_bottom_field{0};
            uint32_t m_num_ref_frames_in_pic_order_cnt_cycle{0};
            std::vector< uint32_t > m_offset_for_ref_frame;

            uint32_t m_max_num_ref_frames{0};
            bool m_gaps_in_frame_num_value_allowed_flag{false};
            uint32_t m_pic_width_in_mbs_minus1;
            uint32_t m_pic_height_in_map_units_minus1;
            bool m_frame_mbs_only_flag{true};
            bool m_mb_adaptive_frame_field_flag{false};
            bool m_direct_8x8_inference_flag{false};
            bool m_frame_cropping_flag{false};
            uint32_t m_frame_crop_left_offset{0};
            uint32_t m_frame_crop_right_offset{0};
            uint32_t m_frame_crop_top_offset{0};
            uint32_t m_frame_crop_bottom_offset{0};
            bool m_vui_parameters_present_flag{false};

            uint32_t m_ChromaArrayType{0};
            int PicWidthInMbs;
            int PicWidthInSamples;
            int PicHeightInMapUnits;
            int FrameHeightInSamples;

        private:
            bool f_has_chroma_format_idc();
            void f_scaling_list( size_t sizeOfScalingList );
        };
    }  // namespace h264
}  // namespace dm
