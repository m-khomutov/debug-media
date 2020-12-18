//
// Created by mkh on 14.12.2020.
//

#pragma once

#include "h264/dmbitreader.h"
#include "h264/parameters/dmsps.h"
#include "h264/parameters/dmpps.h"

#include <cstdint>
#include <cstdlib>
#include <memory>

namespace dm {
    namespace h264 {
        namespace slice {
            class Header {
            public:
                enum SliceType { P, B, I, SP, SI };
                class WeightList {
                public:
                    struct Item {
                        int weight;
                        int offset;
                    };
                    WeightList() = default;
                    WeightList( BitReader & bitreader, int chromaArrayType );

                private:
                    bool m_luma_flag{false};
                    Item m_luma;

                    bool m_chroma_flag{false};
                    Item m_chroma[2];
                };

                Header( const Sps * sps, const Pps * pps, const uint8_t *data, size_t sz );

            private:
                BitReader m_bitreader;
                char m_nal_ref_idc;
                char m_nal_unit_type;

                int m_first_mb_in_slice;
                int m_slice_type;
                bool m_idrPicFlag;
                int m_pic_parameter_set_id;
                int m_colour_plane_id;
                int m_frame_num;
                bool m_field_pic_flag{false};
                bool m_bottom_field_flag{false};
                int m_idr_pic_id;
                int m_pic_order_cnt_lsb;
                int m_delta_pic_order_cnt_bottom;
                int m_delta_pic_order_cnt[2];
                int m_redundant_pic_cnt;
                bool m_direct_spatial_mv_pred_flag;
                bool m_num_ref_idx_active_override_flag;
                int m_num_ref_idx_l0_active_minus1;
                int m_num_ref_idx_l1_active_minus1;
                int m_cabac_init_idc;
                int m_slice_qp_delta;
                bool m_sp_for_switch_flag;
                int m_slice_qs_delta;
                int m_disable_deblocking_filter_idc;
                int m_slice_alpha_c0_offset_div2;
                int m_slice_beta_offset_div2;
                int m_slice_group_change_cycle;

                bool m_ref_pic_list_modification_flag_l0;
                bool m_ref_pic_list_modification_flag_l1;
                std::unique_ptr< int > m_abs_diff_pic_num_minus1;
                std::unique_ptr< int > m_long_term_pic_num;

                bool m_no_output_of_prior_pics_flag;
                bool m_long_term_reference_flag;
                bool m_adaptive_ref_pic_marking_mode_flag;
                std::unique_ptr< int > m_difference_of_pic_nums_minus1;
                std::unique_ptr< int > m_long_term_frame_idx;
                std::unique_ptr< int > m_max_long_term_frame_idx_plus1;

                int m_luma_log2_weight_denom;
                int m_chroma_log2_weight_denom;
                std::vector< WeightList > m_weight_l0;
                std::vector< WeightList > m_weight_l1;

            private:
                void f_ref_pic_list_mvc_modification();
                void f_ref_pic_list_modification();
                void f_pred_weight_table( const Sps * sps );
                void f_dec_ref_pic_marking();
            };
        }  // namespace slice
    }  // namespace h264
}  // namespace dm
