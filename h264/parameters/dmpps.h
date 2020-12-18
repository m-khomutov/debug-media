//
// Created by mkh on 15.12.2020.
//

#pragma once

#include "h264/dmbitreader.h"

namespace dm {
    namespace h264 {
        class Pps {
        public:
            Pps( const uint8_t * data, size_t sz );

            bool bottomFieldPicOrderInFramePresentFlag() const {
                return m_bottom_field_pic_order_in_frame_present_flag;
            }
            bool redundantPicCountPresentFlag() const {
                return m_redundant_pic_cnt_present_flag;
            }
            bool weightedPredFlag() const {
                return m_weighted_pred_flag;
            }
            uint8_t weightedBipredIdc() const {
                return m_weighted_bipred_idc;
            }
            bool entropyCodingModeFlag() const {
                return m_entropy_coding_mode_flag;
            }
            bool deblockingFilterControlPresentFlag() const {
                return m_deblocking_filter_control_present_flag;
            }
            int numSliceGroupsMinus1() const {
                return m_num_slice_groups_minus1;
            }
            int sliceGroupMapType() const {
                return m_slice_group_map_type;
            }
            int sliceGroupChangeRate() const {
                return m_slice_group_change_rate_minus1 + 1;
            }
            int picSizeInMapUnits() const {
                return m_pic_size_in_map_units_minus1 + 1;
            }

        private:
            BitReader m_bitreader;

            int m_pic_parameter_set_id;
            int m_seq_parameter_set_id;
            bool m_entropy_coding_mode_flag;
            bool m_bottom_field_pic_order_in_frame_present_flag;
            int m_num_slice_groups_minus1;
            int m_slice_group_map_type;
            std::vector< int > m_run_length_minus1;
            std::vector< int > m_top_left;
            std::vector< int > m_bottom_right;
            bool m_slice_group_change_direction_flag;
            int m_slice_group_change_rate_minus1;
            int m_pic_size_in_map_units_minus1;
            std::vector< int > m_slice_group_id;
            int m_num_ref_idx_l0_default_active_minus1;
            int m_num_ref_idx_l1_default_active_minus1;
            bool m_weighted_pred_flag;
            uint8_t m_weighted_bipred_idc;
            int m_pic_init_qp_minus26;
            int m_pic_init_qs_minus26;
            int m_chroma_qp_index_offset;
            bool m_deblocking_filter_control_present_flag;
            bool m_constrained_intra_pred_flag;
            bool m_redundant_pic_cnt_present_flag;
            bool m_transform_8x8_mode_flag;
            bool m_pic_scaling_matrix_present_flag;
            bool m_pic_scaling_list_present_flag;
            std::vector< int > m_scaling_list;
            int m_second_chroma_qp_index_offset;

        private:
            void f_read_scaling_list( int sizeOfScalingList, bool useDefaultScalingMatrixFlag );
        };
    }  // namespace h264
}  // namespace dm
