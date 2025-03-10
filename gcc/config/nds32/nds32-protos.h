/* Prototypes for exported functions of Andes NDS32 cpu for GNU compiler
   Copyright (C) 2012-2018 Free Software Foundation, Inc.
   Contributed by Andes Technology Corporation.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */


/* ------------------------------------------------------------------------ */

/* Defining Data Structures for Per-function Information.  */

extern void nds32_init_expanders (void);


/* Register Usage.  */

/* -- Order of Allocation of Registers.  */
extern void nds32_adjust_reg_alloc_order (void);

/* Register Classes.  */

extern enum reg_class nds32_regno_reg_class (int);


/* Stack Layout and Calling Conventions.  */

/* -- Basic Stack Layout.  */

extern rtx nds32_return_addr_rtx (int, rtx);

/* -- Eliminating Frame Pointer and Arg Pointer.  */

extern HOST_WIDE_INT nds32_initial_elimination_offset (unsigned int,
						       unsigned int);

/* -- Passing Arguments in Registers.  */

extern void nds32_init_cumulative_args (CUMULATIVE_ARGS *,
					tree, rtx, tree, int);

/* -- Function Entry and Exit.  */

extern void nds32_expand_prologue (void);
extern void nds32_expand_epilogue (bool);
extern void nds32_expand_prologue_v3push (void);
extern void nds32_expand_epilogue_v3pop (bool);

/* ------------------------------------------------------------------------ */

/* Auxiliary functions for auxiliary macros in nds32.h.  */

extern bool nds32_ls_333_p (rtx, rtx, rtx, machine_mode);

/* Auxiliary functions for lwm/smw.  */

extern bool nds32_valid_smw_lwm_base_p (rtx);

/* Auxiliary functions for expanding rtl used in nds32-multiple.md.  */

extern rtx nds32_expand_load_multiple (int, int, rtx, rtx, bool, rtx *);
extern rtx nds32_expand_store_multiple (int, int, rtx, rtx, bool, rtx *);
extern bool nds32_expand_movmemsi (rtx, rtx, rtx, rtx);
extern bool nds32_expand_setmem (rtx, rtx, rtx, rtx, rtx, rtx);
extern bool nds32_expand_strlen (rtx, rtx, rtx, rtx);

/* Auxiliary functions for expand unalign load instruction.  */

extern void nds32_expand_unaligned_load (rtx *, enum machine_mode);

/* Auxiliary functions for expand unalign store instruction.  */

extern void nds32_expand_unaligned_store (rtx *, enum machine_mode);

/* Auxiliary functions for multiple load/store predicate checking.  */

extern bool nds32_valid_multiple_load_store_p (rtx, bool, bool);

/* Auxiliary functions for stack operation predicate checking.  */

extern bool nds32_valid_stack_push_pop_p (rtx, bool);

/* Auxiliary functions for bit operation detection.  */

extern int nds32_can_use_bclr_p (int);
extern int nds32_can_use_bset_p (int);
extern int nds32_can_use_btgl_p (int);

extern int nds32_can_use_bitci_p (int);

/* Auxiliary function for 'Computing the Length of an Insn'.  */

extern int nds32_adjust_insn_length (rtx_insn *, int);

/* Auxiliary functions for FP_AS_GP detection.  */

extern int nds32_fp_as_gp_check_available (void);

extern bool nds32_symbol_load_store_p (rtx_insn *);

/* Auxiliary functions for jump table generation.  */

extern const char *nds32_output_casesi_pc_relative (rtx *);
extern const char *nds32_output_casesi (rtx *);

/* Auxiliary functions for conditional branch generation.  */

extern enum nds32_expand_result_type nds32_expand_cbranch (rtx *);
extern enum nds32_expand_result_type nds32_expand_cstore (rtx *);

/* Auxiliary functions for conditional move generation.  */

extern enum nds32_expand_result_type nds32_expand_movcc (rtx *);


/* Auxiliary functions to identify long-call symbol.  */
extern bool nds32_long_call_p (rtx);

/* Auxiliary functions to identify 16 bit addresing mode.  */

extern enum nds32_16bit_address_type nds32_mem_format (rtx);

/* Auxiliary functions to output assembly code.  */

extern const char *nds32_output_16bit_store (rtx *, int);
extern const char *nds32_output_16bit_load (rtx *, int);
extern const char *nds32_output_32bit_store (rtx *, int);
extern const char *nds32_output_32bit_load (rtx *, int);
extern const char *nds32_output_32bit_load_s (rtx *, int);
extern const char *nds32_output_smw_single_word (rtx *);
extern const char *nds32_output_lmw_single_word (rtx *);
extern const char *nds32_output_cbranchsi4_equality_zero (rtx_insn *, rtx *);
extern const char *nds32_output_cbranchsi4_equality_reg (rtx_insn *, rtx *);
extern const char *nds32_output_cbranchsi4_equality_reg_or_const_int (rtx_insn *,
								      rtx *);
extern const char *nds32_output_cbranchsi4_greater_less_zero (rtx_insn *, rtx *);


/* Auxiliary functions to output stack push/pop instruction.  */

extern const char *nds32_output_stack_push (rtx);
extern const char *nds32_output_stack_pop (rtx);

/* Auxiliary functions to check using return with null epilogue.  */

extern int nds32_can_use_return_insn (void);

/* Auxiliary functions to decide output alignment or not.  */

extern int nds32_target_alignment (rtx_insn *);

/* Auxiliary functions to expand builtin functions.  */

extern void nds32_init_builtins_impl (void);
extern rtx nds32_expand_builtin_impl (tree, rtx, rtx,
				      machine_mode, int);
extern tree nds32_builtin_decl_impl (unsigned, bool);

/* Auxiliary functions for ISR implementation.  */

extern void nds32_check_isr_attrs_conflict (tree, tree);
extern void nds32_construct_isr_vectors_information (tree, const char *);
extern void nds32_asm_file_start_for_isr (void);
extern void nds32_asm_file_end_for_isr (void);
extern bool nds32_isr_function_p (tree);

/* Auxiliary functions for cost calculation.  */

extern bool nds32_rtx_costs_impl (rtx, machine_mode, int, int, int *, bool);
extern int nds32_address_cost_impl (rtx, machine_mode, addr_space_t, bool);

/* Auxiliary functions for pre-define marco.  */
extern void nds32_cpu_cpp_builtins(struct cpp_reader *);

/* Functions for create nds32 specific optimization pass.  */
extern rtl_opt_pass *make_pass_nds32_relax_opt (gcc::context *);

/* ------------------------------------------------------------------------ */
