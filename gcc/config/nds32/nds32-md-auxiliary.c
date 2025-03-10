/* Auxiliary functions for output asm template or expand rtl
   pattern of Andes NDS32 cpu for GNU compiler
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

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "memmodel.h"
#include "tm_p.h"
#include "optabs.h"		/* For GEN_FCN.  */
#include "recog.h"
#include "output.h"
#include "tm-constrs.h"
#include "expr.h"
#include "emit-rtl.h"
#include "explow.h"

/* ------------------------------------------------------------------------ */

static int
nds32_regno_to_enable4 (unsigned regno)
{
  switch (regno)
    {
    case 28: /* $r28/fp */
      return 0x8;
    case 29: /* $r29/gp */
      return 0x4;
    case 30: /* $r30/lp */
      return 0x2;
    case 31: /* $r31/sp */
      return 0x1;
    default:
      gcc_unreachable ();
    }
}

/* A helper function to return character based on byte size.  */
static char
nds32_byte_to_size (int byte)
{
  switch (byte)
    {
    case 4:
      return 'w';
    case 2:
      return 'h';
    case 1:
      return 'b';
    default:
      /* Normally it should not be here.  */
      gcc_unreachable ();
    }
}

static int
nds32_inverse_cond_code (int code)
{
  switch (code)
    {
      case NE:
	return EQ;
      case EQ:
	return NE;
      case GT:
	return LE;
      case LE:
	return GT;
      case GE:
	return LT;
      case LT:
	return GE;
      default:
	gcc_unreachable ();
    }
}

static const char *
nds32_cond_code_str (int code)
{
  switch (code)
    {
      case NE:
	return "ne";
      case EQ:
	return "eq";
      case GT:
	return "gt";
      case LE:
	return "le";
      case GE:
	return "ge";
      case LT:
	return "lt";
      default:
	gcc_unreachable ();
    }
}

static void
output_cond_branch (int code, const char *suffix, bool r5_p,
		    bool long_jump_p, rtx *operands)
{
  char pattern[256];
  const char *cond_code;

  if (r5_p && REGNO (operands[2]) == 5 && TARGET_16_BIT)
    {
      /* This is special case for beqs38 and bnes38,
	 second operand 2 can't be $r5 and it's almost meanless,
	 however it may occur after copy propgation.  */
      if (code == EQ)
	{
	  /* $r5 == $r5 always taken! */
	  if (long_jump_p)
	    snprintf (pattern, sizeof (pattern),
		      "j\t%%3");
	  else
	    snprintf (pattern, sizeof (pattern),
		      "j8\t%%3");
	}
      else
	/* Don't output anything since $r5 != $r5 never taken! */
	pattern[0] = '\0';
    }
  else if (long_jump_p)
    {
      int inverse_code = nds32_inverse_cond_code (code);
      cond_code = nds32_cond_code_str (inverse_code);

      /*      b<cond><suffix>  $r0, $r1, .L0
	    =>
	      b<inverse_cond><suffix>  $r0, $r1, .LCB0
	      j  .L0
	    .LCB0:

	    or

	      b<cond><suffix>  $r0, $r1, .L0
	    =>
	      b<inverse_cond><suffix>  $r0, $r1, .LCB0
	      j  .L0
	    .LCB0:
      */
      if (r5_p && TARGET_16_BIT)
	{
	  snprintf (pattern, sizeof (pattern),
		    "b%ss38\t %%2, .LCB%%=\n\tj\t%%3\n.LCB%%=:",
		    cond_code);
	}
      else
	{
	  snprintf (pattern, sizeof (pattern),
		    "b%s%s\t%%1, %%2, .LCB%%=\n\tj\t%%3\n.LCB%%=:",
		    cond_code, suffix);
	}
    }
  else
    {
      cond_code = nds32_cond_code_str (code);
      if (r5_p && TARGET_16_BIT)
	{
	  /* b<cond>s38  $r1, .L0   */
	  snprintf (pattern, sizeof (pattern),
		    "b%ss38\t %%2, %%3", cond_code);
	}
      else
	{
	  /* b<cond><suffix>  $r0, $r1, .L0   */
	  snprintf (pattern, sizeof (pattern),
		    "b%s%s\t%%1, %%2, %%3", cond_code, suffix);
	}
    }

  output_asm_insn (pattern, operands);
}

static void
output_cond_branch_compare_zero (int code, const char *suffix,
				 bool long_jump_p, rtx *operands,
				 bool ta_implied_p)
{
  char pattern[256];
  const char *cond_code;
  if (long_jump_p)
    {
      int inverse_code = nds32_inverse_cond_code (code);
      cond_code = nds32_cond_code_str (inverse_code);

      if (ta_implied_p && TARGET_16_BIT)
	{
	  /*    b<cond>z<suffix>  .L0
	      =>
		b<inverse_cond>z<suffix>  .LCB0
		j  .L0
	      .LCB0:
	   */
	  snprintf (pattern, sizeof (pattern),
		    "b%sz%s\t.LCB%%=\n\tj\t%%2\n.LCB%%=:",
		    cond_code, suffix);
	}
      else
	{
	  /*      b<cond>z<suffix>  $r0, .L0
		=>
		  b<inverse_cond>z<suffix>  $r0, .LCB0
		  j  .L0
		.LCB0:
	   */
	  snprintf (pattern, sizeof (pattern),
		    "b%sz%s\t%%1, .LCB%%=\n\tj\t%%2\n.LCB%%=:",
		    cond_code, suffix);
	}
    }
  else
    {
      cond_code = nds32_cond_code_str (code);
      if (ta_implied_p && TARGET_16_BIT)
	{
	  /* b<cond>z<suffix>  .L0  */
	  snprintf (pattern, sizeof (pattern),
		    "b%sz%s\t%%2", cond_code, suffix);
	}
      else
	{
	  /* b<cond>z<suffix>  $r0, .L0  */
	  snprintf (pattern, sizeof (pattern),
		    "b%sz%s\t%%1, %%2", cond_code, suffix);
	}
    }

  output_asm_insn (pattern, operands);
}

/* ------------------------------------------------------------------------ */

/* Auxiliary function for expand RTL pattern.  */

enum nds32_expand_result_type
nds32_expand_cbranch (rtx *operands)
{
  rtx tmp_reg;
  enum rtx_code code;

  code = GET_CODE (operands[0]);

  /* If operands[2] is (const_int 0),
     we can use beqz,bnez,bgtz,bgez,bltz,or blez instructions.
     So we have gcc generate original template rtx.  */
  if (GET_CODE (operands[2]) == CONST_INT)
    if (INTVAL (operands[2]) == 0)
      if ((code != GTU)
	  && (code != GEU)
	  && (code != LTU)
	  && (code != LEU))
	return EXPAND_CREATE_TEMPLATE;

  /* For other comparison, NDS32 ISA only has slt (Set-on-Less-Than)
     behavior for the comparison, we might need to generate other
     rtx patterns to achieve same semantic.  */
  switch (code)
    {
    case GT:
    case GTU:
      if (GET_CODE (operands[2]) == CONST_INT)
	{
	  /* GT  reg_A, const_int  =>  !(LT  reg_A, const_int + 1) */
	  if (optimize_size || optimize == 0)
	    tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
	  else
	    tmp_reg = gen_reg_rtx (SImode);

	  /* We want to plus 1 into the integer value
	     of operands[2] to create 'slt' instruction.
	     This caculation is performed on the host machine,
	     which may be 64-bit integer.
	     So the meaning of caculation result may be
	     different from the 32-bit nds32 target.

	     For example:
	       0x7fffffff + 0x1 -> 0x80000000,
	       this value is POSITIVE on 64-bit machine,
	       but the expected value on 32-bit nds32 target
	       should be NEGATIVE value.

	     Hence, instead of using GEN_INT(), we use gen_int_mode() to
	     explicitly create SImode constant rtx.  */
	  enum rtx_code cmp_code;

	  rtx plus1 = gen_int_mode (INTVAL (operands[2]) + 1, SImode);
	  if (satisfies_constraint_Is15 (plus1))
	    {
	      operands[2] = plus1;
	      cmp_code = EQ;
	      if (code == GT)
		{
		  /* GT, use slts instruction */
		  emit_insn (
		    gen_slts_compare (tmp_reg, operands[1], operands[2]));
		}
	      else
		{
		  /* GTU, use slt instruction */
		  emit_insn (
		    gen_slt_compare  (tmp_reg, operands[1], operands[2]));
		}
	    }
	  else
	    {
	      cmp_code = NE;
	      if (code == GT)
		{
		  /* GT, use slts instruction */
		  emit_insn (
		    gen_slts_compare (tmp_reg, operands[2], operands[1]));
		}
	      else
		{
		  /* GTU, use slt instruction */
		  emit_insn (
		    gen_slt_compare  (tmp_reg, operands[2], operands[1]));
		}
	    }

	  PUT_CODE (operands[0], cmp_code);
	  operands[1] = tmp_reg;
	  operands[2] = const0_rtx;
	  emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				     operands[2], operands[3]));

	  return EXPAND_DONE;
	}
      else
	{
	  /* GT  reg_A, reg_B  =>  LT  reg_B, reg_A */
	  if (optimize_size || optimize == 0)
	    tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
	  else
	    tmp_reg = gen_reg_rtx (SImode);

	  if (code == GT)
	    {
	      /* GT, use slts instruction */
	      emit_insn (gen_slts_compare (tmp_reg, operands[2], operands[1]));
	    }
	  else
	    {
	      /* GTU, use slt instruction */
	      emit_insn (gen_slt_compare  (tmp_reg, operands[2], operands[1]));
	    }

	  PUT_CODE (operands[0], NE);
	  operands[1] = tmp_reg;
	  operands[2] = const0_rtx;
	  emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				     operands[2], operands[3]));

	  return EXPAND_DONE;
	}

    case GE:
    case GEU:
      /* GE  reg_A, reg_B      =>  !(LT  reg_A, reg_B) */
      /* GE  reg_A, const_int  =>  !(LT  reg_A, const_int) */
      if (optimize_size || optimize == 0)
	tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
      else
	tmp_reg = gen_reg_rtx (SImode);

      if (code == GE)
	{
	  /* GE, use slts instruction */
	  emit_insn (gen_slts_compare (tmp_reg, operands[1], operands[2]));
	}
      else
	{
	  /* GEU, use slt instruction */
	  emit_insn (gen_slt_compare  (tmp_reg, operands[1], operands[2]));
	}

      PUT_CODE (operands[0], EQ);
      operands[1] = tmp_reg;
      operands[2] = const0_rtx;
      emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				 operands[2], operands[3]));

      return EXPAND_DONE;

    case LT:
    case LTU:
      /* LT  reg_A, reg_B      =>  LT  reg_A, reg_B */
      /* LT  reg_A, const_int  =>  LT  reg_A, const_int */
      if (optimize_size || optimize == 0)
	tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
      else
	tmp_reg = gen_reg_rtx (SImode);

      if (code == LT)
	{
	  /* LT, use slts instruction */
	  emit_insn (gen_slts_compare (tmp_reg, operands[1], operands[2]));
	}
      else
	{
	  /* LTU, use slt instruction */
	  emit_insn (gen_slt_compare  (tmp_reg, operands[1], operands[2]));
	}

      PUT_CODE (operands[0], NE);
      operands[1] = tmp_reg;
      operands[2] = const0_rtx;
      emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				 operands[2], operands[3]));

      return EXPAND_DONE;

    case LE:
    case LEU:
      if (GET_CODE (operands[2]) == CONST_INT)
	{
	  /* LE  reg_A, const_int  =>  LT  reg_A, const_int + 1 */
	  if (optimize_size || optimize == 0)
	    tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
	  else
	    tmp_reg = gen_reg_rtx (SImode);

	  enum rtx_code cmp_code;
	  /* Note that (le:SI X INT_MAX) is not the same as (lt:SI X INT_MIN).
	     We better have an assert here in case GCC does not properly
	     optimize it away.  The INT_MAX here is 0x7fffffff for target.  */
	  rtx plus1 = gen_int_mode (INTVAL (operands[2]) + 1, SImode);
	  if (satisfies_constraint_Is15 (plus1))
	    {
	      operands[2] = plus1;
	      cmp_code = NE;
	      if (code == LE)
		{
		  /* LE, use slts instruction */
		  emit_insn (
		    gen_slts_compare (tmp_reg, operands[1], operands[2]));
		}
	      else
		{
		  /* LEU, use slt instruction */
		  emit_insn (
		    gen_slt_compare  (tmp_reg, operands[1], operands[2]));
		}
	    }
	  else
	    {
	      cmp_code = EQ;
	      if (code == LE)
		{
		  /* LE, use slts instruction */
		  emit_insn (
		    gen_slts_compare (tmp_reg, operands[2], operands[1]));
		}
	      else
		{
		  /* LEU, use slt instruction */
		  emit_insn (
		    gen_slt_compare  (tmp_reg, operands[2], operands[1]));
		}
	    }

	  PUT_CODE (operands[0], cmp_code);
	  operands[1] = tmp_reg;
	  operands[2] = const0_rtx;
	  emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				     operands[2], operands[3]));

	  return EXPAND_DONE;
	}
      else
	{
	  /* LE  reg_A, reg_B  =>  !(LT  reg_B, reg_A) */
	  if (optimize_size || optimize == 0)
	    tmp_reg = gen_rtx_REG (SImode, TA_REGNUM);
	  else
	    tmp_reg = gen_reg_rtx (SImode);

	  if (code == LE)
	    {
	      /* LE, use slts instruction */
	      emit_insn (gen_slts_compare (tmp_reg, operands[2], operands[1]));
	    }
	  else
	    {
	      /* LEU, use slt instruction */
	      emit_insn (gen_slt_compare  (tmp_reg, operands[2], operands[1]));
	    }

	  PUT_CODE (operands[0], EQ);
	  operands[1] = tmp_reg;
	  operands[2] = const0_rtx;
	  emit_insn (gen_cbranchsi4 (operands[0], operands[1],
				     operands[2], operands[3]));

	  return EXPAND_DONE;
	}

    case EQ:
    case NE:
      /* NDS32 ISA has various form for eq/ne behavior no matter
	 what kind of the operand is.
	 So just generate original template rtx.  */

      /* Put operands[2] into register if operands[2] is a large
	 const_int or ISAv2.  */
      if (GET_CODE (operands[2]) == CONST_INT
	  && (!satisfies_constraint_Is11 (operands[2])
	      || TARGET_ISA_V2))
	operands[2] = force_reg (SImode, operands[2]);

      return EXPAND_CREATE_TEMPLATE;

    default:
      return EXPAND_FAIL;
    }
}

enum nds32_expand_result_type
nds32_expand_cstore (rtx *operands)
{
  rtx tmp_reg;
  enum rtx_code code;

  code = GET_CODE (operands[1]);

  switch (code)
    {
    case EQ:
    case NE:
      if (GET_CODE (operands[3]) == CONST_INT)
	{
	  /* reg_R = (reg_A == const_int_B)
	     --> xori reg_C, reg_A, const_int_B
		 slti reg_R, reg_C, const_int_1
	     reg_R = (reg_A != const_int_B)
	     --> xori reg_C, reg_A, const_int_B
		 slti reg_R, const_int0, reg_C */
	  tmp_reg = gen_reg_rtx (SImode);

	  /* If the integer value is not in the range of imm15s,
	     we need to force register first because our addsi3 pattern
	     only accept nds32_rimm15s_operand predicate.  */
	  rtx new_imm = gen_int_mode (-INTVAL (operands[3]), SImode);
	  if (satisfies_constraint_Is15 (new_imm))
	    emit_insn (gen_addsi3 (tmp_reg, operands[2], new_imm));
	  else
	    {
	      if (!(satisfies_constraint_Iu15 (operands[3])
		    || (TARGET_EXT_PERF
			&& satisfies_constraint_It15 (operands[3]))))
		operands[3] = force_reg (SImode, operands[3]);
	      emit_insn (gen_xorsi3 (tmp_reg, operands[2], operands[3]));
	    }

	  if (code == EQ)
	    emit_insn (gen_slt_eq0 (operands[0], tmp_reg));
	  else
	    emit_insn (gen_slt_compare (operands[0], const0_rtx, tmp_reg));

	  return EXPAND_DONE;
	}
      else
	{
	  /* reg_R = (reg_A == reg_B)
	     --> xor  reg_C, reg_A, reg_B
		 slti reg_R, reg_C, const_int_1
	     reg_R = (reg_A != reg_B)
	     --> xor  reg_C, reg_A, reg_B
		 slti reg_R, const_int0, reg_C */
	  tmp_reg = gen_reg_rtx (SImode);
	  emit_insn (gen_xorsi3 (tmp_reg, operands[2], operands[3]));
	  if (code == EQ)
	    emit_insn (gen_slt_eq0 (operands[0], tmp_reg));
	  else
	    emit_insn (gen_slt_compare (operands[0], const0_rtx, tmp_reg));

	  return EXPAND_DONE;
	}
    case GT:
    case GTU:
      /* reg_R = (reg_A > reg_B)       --> slt reg_R, reg_B, reg_A */
      /* reg_R = (reg_A > const_int_B) --> slt reg_R, const_int_B, reg_A */
      if (code == GT)
	{
	  /* GT, use slts instruction */
	  emit_insn (gen_slts_compare (operands[0], operands[3], operands[2]));
	}
      else
	{
	  /* GTU, use slt instruction */
	  emit_insn (gen_slt_compare  (operands[0], operands[3], operands[2]));
	}

      return EXPAND_DONE;

    case GE:
    case GEU:
      if (GET_CODE (operands[3]) == CONST_INT)
	{
	  /* reg_R = (reg_A >= const_int_B)
	     --> movi reg_C, const_int_B - 1
		 slt  reg_R, reg_C, reg_A */
	  tmp_reg = gen_reg_rtx (SImode);

	  emit_insn (gen_movsi (tmp_reg,
				gen_int_mode (INTVAL (operands[3]) - 1,
					      SImode)));
	  if (code == GE)
	    {
	      /* GE, use slts instruction */
	      emit_insn (gen_slts_compare (operands[0], tmp_reg, operands[2]));
	    }
	  else
	    {
	      /* GEU, use slt instruction */
	      emit_insn (gen_slt_compare  (operands[0], tmp_reg, operands[2]));
	    }

	  return EXPAND_DONE;
	}
      else
	{
	  /* reg_R = (reg_A >= reg_B)
	     --> slt  reg_R, reg_A, reg_B
		 xori reg_R, reg_R, const_int_1 */
	  if (code == GE)
	    {
	      /* GE, use slts instruction */
	      emit_insn (gen_slts_compare (operands[0],
					   operands[2], operands[3]));
	    }
	  else
	    {
	      /* GEU, use slt instruction */
	      emit_insn (gen_slt_compare  (operands[0],
					   operands[2], operands[3]));
	    }

	  /* perform 'not' behavior */
	  emit_insn (gen_xorsi3 (operands[0], operands[0], const1_rtx));

	  return EXPAND_DONE;
	}

    case LT:
    case LTU:
      /* reg_R = (reg_A < reg_B)       --> slt reg_R, reg_A, reg_B */
      /* reg_R = (reg_A < const_int_B) --> slt reg_R, reg_A, const_int_B */
      if (code == LT)
	{
	  /* LT, use slts instruction */
	  emit_insn (gen_slts_compare (operands[0], operands[2], operands[3]));
	}
      else
	{
	  /* LTU, use slt instruction */
	  emit_insn (gen_slt_compare  (operands[0], operands[2], operands[3]));
	}

      return EXPAND_DONE;

    case LE:
    case LEU:
      if (GET_CODE (operands[3]) == CONST_INT)
	{
	  /* reg_R = (reg_A <= const_int_B)
	     --> movi reg_C, const_int_B + 1
		 slt  reg_R, reg_A, reg_C */
	  tmp_reg = gen_reg_rtx (SImode);

	  emit_insn (gen_movsi (tmp_reg,
				gen_int_mode (INTVAL (operands[3]) + 1,
						      SImode)));
	  if (code == LE)
	    {
	      /* LE, use slts instruction */
	      emit_insn (gen_slts_compare (operands[0], operands[2], tmp_reg));
	    }
	  else
	    {
	      /* LEU, use slt instruction */
	      emit_insn (gen_slt_compare  (operands[0], operands[2], tmp_reg));
	    }

	  return EXPAND_DONE;
	}
      else
	{
	  /* reg_R = (reg_A <= reg_B) --> slt  reg_R, reg_B, reg_A
					  xori reg_R, reg_R, const_int_1 */
	  if (code == LE)
	    {
	      /* LE, use slts instruction */
	      emit_insn (gen_slts_compare (operands[0],
					   operands[3], operands[2]));
	    }
	  else
	    {
	      /* LEU, use slt instruction */
	      emit_insn (gen_slt_compare  (operands[0],
					   operands[3], operands[2]));
	    }

	  /* perform 'not' behavior */
	  emit_insn (gen_xorsi3 (operands[0], operands[0], const1_rtx));

	  return EXPAND_DONE;
	}


    default:
      gcc_unreachable ();
    }
}

enum nds32_expand_result_type
nds32_expand_movcc (rtx *operands)
{
  enum rtx_code code = GET_CODE (operands[1]);
  enum rtx_code new_code = code;
  machine_mode cmp0_mode = GET_MODE (XEXP (operands[1], 0));
  rtx cmp_op0 = XEXP (operands[1], 0);
  rtx cmp_op1 = XEXP (operands[1], 1);
  rtx tmp;

  if ((GET_CODE (operands[1]) == EQ || GET_CODE (operands[1]) == NE)
      && XEXP (operands[1], 1) == const0_rtx)
    {
      /* If the operands[1] rtx is already (eq X 0) or (ne X 0),
	 we have gcc generate original template rtx.  */
      return EXPAND_CREATE_TEMPLATE;
    }
  else
    {
      /* Since there is only 'slt'(Set when Less Than) instruction for
	 comparison in Andes ISA, the major strategy we use here is to
	 convert conditional move into 'LT + EQ' or 'LT + NE' rtx combination.
	 We design constraints properly so that the reload phase will assist
	 to make one source operand to use same register as result operand.
	 Then we can use cmovz/cmovn to catch the other source operand
	 which has different register.  */
      int reverse = 0;

      /* Main Goal: Use 'LT + EQ' or 'LT + NE' to target "then" part
	 Strategy : Reverse condition and swap comparison operands

	 For example:

	     a <= b ? P : Q   (LE or LEU)
	 --> a >  b ? Q : P   (reverse condition)
	 --> b <  a ? Q : P   (swap comparison operands to achieve 'LT/LTU')

	     a >= b ? P : Q   (GE or GEU)
	 --> a <  b ? Q : P   (reverse condition to achieve 'LT/LTU')

	     a <  b ? P : Q   (LT or LTU)
	 --> (NO NEED TO CHANGE, it is already 'LT/LTU')

	     a >  b ? P : Q   (GT or GTU)
	 --> b <  a ? P : Q   (swap comparison operands to achieve 'LT/LTU') */
      switch (code)
	{
	case GE: case GEU: case LE: case LEU:
	  new_code = reverse_condition (code);
	  reverse = 1;
	  break;
	case EQ:
	case NE:
	  /* no need to reverse condition */
	  break;
	default:
	  return EXPAND_FAIL;
	}

      /* For '>' comparison operator, we swap operands
	 so that we can have 'LT/LTU' operator.  */
      if (new_code == GT || new_code == GTU)
	{
	  tmp     = cmp_op0;
	  cmp_op0 = cmp_op1;
	  cmp_op1 = tmp;

	  new_code = swap_condition (new_code);
	}

      /* Use a temporary register to store slt/slts result.  */
      tmp = gen_reg_rtx (SImode);

      if (new_code == EQ || new_code == NE)
	{
	  emit_insn (gen_xorsi3 (tmp, cmp_op0, cmp_op1));
	  /* tmp == 0 if cmp_op0 == cmp_op1.  */
	  operands[1] = gen_rtx_fmt_ee (new_code, VOIDmode, tmp, const0_rtx);
	}
      else
	{
	  /* This emit_insn will create corresponding 'slt/slts'
	      insturction.  */
	  if (new_code == LT)
	    emit_insn (gen_slts_compare (tmp, cmp_op0, cmp_op1));
	  else if (new_code == LTU)
	    emit_insn (gen_slt_compare (tmp, cmp_op0, cmp_op1));
	  else
	    gcc_unreachable ();

	  /* Change comparison semantic into (eq X 0) or (ne X 0) behavior
	     so that cmovz or cmovn will be matched later.

	     For reverse condition cases, we want to create a semantic that:
	       (eq X 0) --> pick up "else" part
	     For normal cases, we want to create a semantic that:
	       (ne X 0) --> pick up "then" part

	     Later we will have cmovz/cmovn instruction pattern to
	     match corresponding behavior and output instruction.  */
	  operands[1] = gen_rtx_fmt_ee (reverse ? EQ : NE,
					VOIDmode, tmp, const0_rtx);
	}
    }
  return EXPAND_CREATE_TEMPLATE;
}

/* ------------------------------------------------------------------------ */

/* Function to return memory format.  */
enum nds32_16bit_address_type
nds32_mem_format (rtx op)
{
  machine_mode mode_test;
  int val;
  int regno;

  if (!TARGET_16_BIT)
    return ADDRESS_NOT_16BIT_FORMAT;

  mode_test = GET_MODE (op);

  op = XEXP (op, 0);

  /* 45 format.  */
  if (GET_CODE (op) == REG && (mode_test == SImode))
    return ADDRESS_REG;

  /* 333 format for QI/HImode.  */
  if (GET_CODE (op) == REG && (REGNO (op) < R8_REGNUM))
    return ADDRESS_LO_REG_IMM3U;

  /* post_inc 333 format.  */
  if ((GET_CODE (op) == POST_INC) && (mode_test == SImode))
    {
      regno = REGNO(XEXP (op, 0));

      if (regno < 8)
	return ADDRESS_POST_INC_LO_REG_IMM3U;
    }

  /* post_inc 333 format.  */
  if ((GET_CODE (op) == POST_MODIFY)
      && (mode_test == SImode)
      && (REG_P (XEXP (XEXP (op, 1), 0)))
      && (CONST_INT_P (XEXP (XEXP (op, 1), 1))))
    {
      regno = REGNO (XEXP (XEXP (op, 1), 0));
      val = INTVAL (XEXP (XEXP (op, 1), 1));
      if (regno < 8 && val < 32)
	return ADDRESS_POST_INC_LO_REG_IMM3U;
    }

  if ((GET_CODE (op) == PLUS)
      && (GET_CODE (XEXP (op, 0)) == REG)
      && (GET_CODE (XEXP (op, 1)) == CONST_INT))
    {
      val = INTVAL (XEXP (op, 1));

      regno = REGNO(XEXP (op, 0));

      if (regno > 7
	  && regno != SP_REGNUM
	  && regno != FP_REGNUM)
	return ADDRESS_NOT_16BIT_FORMAT;

      switch (mode_test)
	{
	case E_QImode:
	  /* 333 format.  */
	  if (val >= 0 && val < 8 && regno < 8)
	    return ADDRESS_LO_REG_IMM3U;
	  break;

	case E_HImode:
	  /* 333 format.  */
	  if (val >= 0 && val < 16 && (val % 2 == 0) && regno < 8)
	    return ADDRESS_LO_REG_IMM3U;
	  break;

	case E_SImode:
	case E_SFmode:
	case E_DFmode:
	  /* fp imply 37 format.  */
	  if ((regno == FP_REGNUM) &&
	      (val >= 0 && val < 512 && (val % 4 == 0)))
	    return ADDRESS_FP_IMM7U;
	  /* sp imply 37 format.  */
	  else if ((regno == SP_REGNUM) &&
		   (val >= 0 && val < 512 && (val % 4 == 0)))
	    return ADDRESS_SP_IMM7U;
	  /* 333 format.  */
	  else if (val >= 0 && val < 32 && (val % 4 == 0) && regno < 8)
	    return ADDRESS_LO_REG_IMM3U;
	  break;

	default:
	  break;
	}
    }

  return ADDRESS_NOT_16BIT_FORMAT;
}

/* Output 16-bit store.  */
const char *
nds32_output_16bit_store (rtx *operands, int byte)
{
  char pattern[100];
  char size;
  rtx code = XEXP (operands[0], 0);

  size = nds32_byte_to_size (byte);

  switch (nds32_mem_format (operands[0]))
    {
    case ADDRESS_REG:
      operands[0] = code;
      output_asm_insn ("swi450\t%1, [%0]", operands);
      break;
    case ADDRESS_LO_REG_IMM3U:
      snprintf (pattern, sizeof (pattern), "s%ci333\t%%1, %%0", size);
      output_asm_insn (pattern, operands);
      break;
    case ADDRESS_POST_INC_LO_REG_IMM3U:
      snprintf (pattern, sizeof (pattern), "s%ci333.bi\t%%1, %%0", size);
      output_asm_insn (pattern, operands);
      break;
    case ADDRESS_FP_IMM7U:
      output_asm_insn ("swi37\t%1, %0", operands);
      break;
    case ADDRESS_SP_IMM7U:
      /* Get immediate value and set back to operands[1].  */
      operands[0] = XEXP (code, 1);
      output_asm_insn ("swi37.sp\t%1, [ + (%0)]", operands);
      break;
    default:
      break;
    }

  return "";
}

/* Output 16-bit load.  */
const char *
nds32_output_16bit_load (rtx *operands, int byte)
{
  char pattern[100];
  unsigned char size;
  rtx code = XEXP (operands[1], 0);

  size = nds32_byte_to_size (byte);

  switch (nds32_mem_format (operands[1]))
    {
    case ADDRESS_REG:
      operands[1] = code;
      output_asm_insn ("lwi450\t%0, [%1]", operands);
      break;
    case ADDRESS_LO_REG_IMM3U:
      snprintf (pattern, sizeof (pattern), "l%ci333\t%%0, %%1", size);
      output_asm_insn (pattern, operands);
      break;
    case ADDRESS_POST_INC_LO_REG_IMM3U:
      snprintf (pattern, sizeof (pattern), "l%ci333.bi\t%%0, %%1", size);
      output_asm_insn (pattern, operands);
      break;
    case ADDRESS_FP_IMM7U:
      output_asm_insn ("lwi37\t%0, %1", operands);
      break;
    case ADDRESS_SP_IMM7U:
      /* Get immediate value and set back to operands[0].  */
      operands[1] = XEXP (code, 1);
      output_asm_insn ("lwi37.sp\t%0, [ + (%1)]", operands);
      break;
    default:
      break;
    }

  return "";
}

/* Output 32-bit store.  */
const char *
nds32_output_32bit_store (rtx *operands, int byte)
{
  char pattern[100];
  unsigned char size;
  rtx code = XEXP (operands[0], 0);

  size = nds32_byte_to_size (byte);

  switch (GET_CODE (code))
    {
    case REG:
      /* (mem (reg X))
	 => access location by using register,
	 use "sbi / shi / swi" */
      snprintf (pattern, sizeof (pattern), "s%ci\t%%1, %%0", size);
      break;

    case SYMBOL_REF:
    case CONST:
      /* (mem (symbol_ref X))
	 (mem (const (...)))
	 => access global variables,
	 use "sbi.gp / shi.gp / swi.gp" */
      operands[0] = XEXP (operands[0], 0);
      snprintf (pattern, sizeof (pattern), "s%ci.gp\t%%1, [ + %%0]", size);
      break;

    case POST_INC:
      /* (mem (post_inc reg))
	 => access location by using register which will be post increment,
	 use "sbi.bi / shi.bi / swi.bi" */
      snprintf (pattern, sizeof (pattern),
		"s%ci.bi\t%%1, %%0, %d", size, byte);
      break;

    case POST_DEC:
      /* (mem (post_dec reg))
	 => access location by using register which will be post decrement,
	 use "sbi.bi / shi.bi / swi.bi" */
      snprintf (pattern, sizeof (pattern),
		"s%ci.bi\t%%1, %%0, -%d", size, byte);
      break;

    case POST_MODIFY:
      switch (GET_CODE (XEXP (XEXP (code, 1), 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (post_modify (reg) (plus (reg) (reg))))
	     => access location by using register which will be
	     post modified with reg,
	     use "sb.bi/ sh.bi / sw.bi" */
	  snprintf (pattern, sizeof (pattern), "s%c.bi\t%%1, %%0", size);
	  break;
	case CONST_INT:
	  /* (mem (post_modify (reg) (plus (reg) (const_int))))
	     => access location by using register which will be
	     post modified with const_int,
	     use "sbi.bi/ shi.bi / swi.bi" */
	  snprintf (pattern, sizeof (pattern), "s%ci.bi\t%%1, %%0", size);
	  break;
	default:
	  abort ();
	}
      break;

    case PLUS:
      switch (GET_CODE (XEXP (code, 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (plus reg reg)) or (mem (plus (mult reg const_int) reg))
	     => access location by adding two registers,
	     use "sb / sh / sw" */
	  snprintf (pattern, sizeof (pattern), "s%c\t%%1, %%0", size);
	  break;
	case CONST_INT:
	  /* (mem (plus reg const_int))
	     => access location by adding one register with const_int,
	     use "sbi / shi / swi" */
	  snprintf (pattern, sizeof (pattern), "s%ci\t%%1, %%0", size);
	  break;
	default:
	  abort ();
	}
      break;

    case LO_SUM:
      operands[2] = XEXP (code, 1);
      operands[0] = XEXP (code, 0);
      snprintf (pattern, sizeof (pattern),
		"s%ci\t%%1, [%%0 + lo12(%%2)]", size);
      break;

    default:
      abort ();
    }

  output_asm_insn (pattern, operands);
  return "";
}

/* Output 32-bit load.  */
const char *
nds32_output_32bit_load (rtx *operands, int byte)
{
  char pattern[100];
  unsigned char size;
  rtx code;

  code = XEXP (operands[1], 0);

  size = nds32_byte_to_size (byte);

  switch (GET_CODE (code))
    {
    case REG:
      /* (mem (reg X))
	 => access location by using register,
	 use "lbi / lhi / lwi" */
      snprintf (pattern, sizeof (pattern), "l%ci\t%%0, %%1", size);
      break;

    case SYMBOL_REF:
    case CONST:
      /* (mem (symbol_ref X))
	 (mem (const (...)))
	 => access global variables,
	 use "lbi.gp / lhi.gp / lwi.gp" */
      operands[1] = XEXP (operands[1], 0);
      snprintf (pattern, sizeof (pattern), "l%ci.gp\t%%0, [ + %%1]", size);
      break;

    case POST_INC:
      /* (mem (post_inc reg))
	 => access location by using register which will be post increment,
	 use "lbi.bi / lhi.bi / lwi.bi" */
      snprintf (pattern, sizeof (pattern),
		"l%ci.bi\t%%0, %%1, %d", size, byte);
      break;

    case POST_DEC:
      /* (mem (post_dec reg))
	 => access location by using register which will be post decrement,
	 use "lbi.bi / lhi.bi / lwi.bi" */
      snprintf (pattern, sizeof (pattern),
		"l%ci.bi\t%%0, %%1, -%d", size, byte);
      break;

    case POST_MODIFY:
      switch (GET_CODE (XEXP (XEXP (code, 1), 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (post_modify (reg) (plus (reg) (reg))))
	     => access location by using register which will be
	     post modified with reg,
	     use "lb.bi/ lh.bi / lw.bi" */
	  snprintf (pattern, sizeof (pattern), "l%c.bi\t%%0, %%1", size);
	  break;
	case CONST_INT:
	  /* (mem (post_modify (reg) (plus (reg) (const_int))))
	     => access location by using register which will be
	     post modified with const_int,
	     use "lbi.bi/ lhi.bi / lwi.bi" */
	  snprintf (pattern, sizeof (pattern), "l%ci.bi\t%%0, %%1", size);
	  break;
	default:
	  abort ();
	}
      break;

    case PLUS:
      switch (GET_CODE (XEXP (code, 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (plus reg reg)) or (mem (plus (mult reg const_int) reg))
	     use "lb / lh / lw" */
	  snprintf (pattern, sizeof (pattern), "l%c\t%%0, %%1", size);
	  break;
	case CONST_INT:
	  /* (mem (plus reg const_int))
	     => access location by adding one register with const_int,
	     use "lbi / lhi / lwi" */
	  snprintf (pattern, sizeof (pattern), "l%ci\t%%0, %%1", size);
	  break;
	default:
	  abort ();
	}
      break;

    case LO_SUM:
      operands[2] = XEXP (code, 1);
      operands[1] = XEXP (code, 0);
      snprintf (pattern, sizeof (pattern),
		"l%ci\t%%0, [%%1 + lo12(%%2)]", size);
      break;

    default:
      abort ();
    }

  output_asm_insn (pattern, operands);
  return "";
}

/* Output 32-bit load with signed extension.  */
const char *
nds32_output_32bit_load_s (rtx *operands, int byte)
{
  char pattern[100];
  unsigned char size;
  rtx code;

  code = XEXP (operands[1], 0);

  size = nds32_byte_to_size (byte);

  switch (GET_CODE (code))
    {
    case REG:
      /* (mem (reg X))
	 => access location by using register,
	 use "lbsi / lhsi" */
      snprintf (pattern, sizeof (pattern), "l%csi\t%%0, %%1", size);
      break;

    case SYMBOL_REF:
    case CONST:
      /* (mem (symbol_ref X))
	 (mem (const (...)))
	 => access global variables,
	 use "lbsi.gp / lhsi.gp" */
      operands[1] = XEXP (operands[1], 0);
      snprintf (pattern, sizeof (pattern), "l%csi.gp\t%%0, [ + %%1]", size);
      break;

    case POST_INC:
      /* (mem (post_inc reg))
	 => access location by using register which will be post increment,
	 use "lbsi.bi / lhsi.bi" */
      snprintf (pattern, sizeof (pattern),
		"l%csi.bi\t%%0, %%1, %d", size, byte);
      break;

    case POST_DEC:
      /* (mem (post_dec reg))
	 => access location by using register which will be post decrement,
	 use "lbsi.bi / lhsi.bi" */
      snprintf (pattern, sizeof (pattern),
		"l%csi.bi\t%%0, %%1, -%d", size, byte);
      break;

    case POST_MODIFY:
      switch (GET_CODE (XEXP (XEXP (code, 1), 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (post_modify (reg) (plus (reg) (reg))))
	     => access location by using register which will be
	     post modified with reg,
	     use "lbs.bi/ lhs.bi" */
	  snprintf (pattern, sizeof (pattern), "l%cs.bi\t%%0, %%1", size);
	  break;
	case CONST_INT:
	  /* (mem (post_modify (reg) (plus (reg) (const_int))))
	     => access location by using register which will be
	     post modified with const_int,
	     use "lbsi.bi/ lhsi.bi" */
	  snprintf (pattern, sizeof (pattern), "l%csi.bi\t%%0, %%1", size);
	  break;
	default:
	  abort ();
	}
      break;

    case PLUS:
      switch (GET_CODE (XEXP (code, 1)))
	{
	case REG:
	case SUBREG:
	  /* (mem (plus reg reg)) or (mem (plus (mult reg const_int) reg))
	     use "lbs / lhs" */
	  snprintf (pattern, sizeof (pattern), "l%cs\t%%0, %%1", size);
	  break;
	case CONST_INT:
	  /* (mem (plus reg const_int))
	     => access location by adding one register with const_int,
	     use "lbsi / lhsi" */
	  snprintf (pattern, sizeof (pattern), "l%csi\t%%0, %%1", size);
	  break;
	default:
	  abort ();
	}
      break;

    case LO_SUM:
      operands[2] = XEXP (code, 1);
      operands[1] = XEXP (code, 0);
      snprintf (pattern, sizeof (pattern),
		"l%csi\t%%0, [%%1 + lo12(%%2)]", size);
      break;

    default:
      abort ();
    }

  output_asm_insn (pattern, operands);
  return "";
}

/* Function to output stack push operation.
   We need to deal with normal stack push multiple or stack v3push.  */
const char *
nds32_output_stack_push (rtx par_rtx)
{
  /* A string pattern for output_asm_insn().  */
  char pattern[100];
  /* The operands array which will be used in output_asm_insn().  */
  rtx operands[3];
  /* Pick up varargs first regno and last regno for further use.  */
  int rb_va_args = cfun->machine->va_args_first_regno;
  int re_va_args = cfun->machine->va_args_last_regno;
  int last_argument_regno = NDS32_FIRST_GPR_REGNUM
			    + NDS32_MAX_GPR_REGS_FOR_ARGS
			    - 1;
  /* Pick up callee-saved first regno and last regno for further use.  */
  int rb_callee_saved = cfun->machine->callee_saved_first_gpr_regno;
  int re_callee_saved = cfun->machine->callee_saved_last_gpr_regno;

  /* First we need to check if we are pushing argument registers not used
     for the named arguments.  If so, we have to create 'smw.adm' (push.s)
     instruction.  */
  if (reg_mentioned_p (gen_rtx_REG (SImode, last_argument_regno), par_rtx))
    {
      /* Set operands[0] and operands[1].  */
      operands[0] = gen_rtx_REG (SImode, rb_va_args);
      operands[1] = gen_rtx_REG (SImode, re_va_args);
      /* Create assembly code pattern: "Rb, Re, { }".  */
      snprintf (pattern, sizeof (pattern), "push.s\t%s", "%0, %1, { }");
      /* We use output_asm_insn() to output assembly code by ourself.  */
      output_asm_insn (pattern, operands);
      return "";
    }

  /* If we step here, we are going to do v3push or multiple push operation.  */

  /* The v3push/v3pop instruction should only be applied on
     none-isr and none-variadic function.  */
  if (TARGET_V3PUSH
      && !nds32_isr_function_p (current_function_decl)
      && (cfun->machine->va_args_size == 0))
    {
      /* For stack v3push:
	   operands[0]: Re
	   operands[1]: imm8u */

      /* This variable is to check if 'push25 Re,imm8u' is available.  */
      int sp_adjust;

      /* Set operands[0].  */
      operands[0] = gen_rtx_REG (SImode, re_callee_saved);

      /* Check if we can generate 'push25 Re,imm8u',
	 otherwise, generate 'push25 Re,0'.  */
      sp_adjust = cfun->machine->local_size
		  + cfun->machine->out_args_size
		  + cfun->machine->callee_saved_area_gpr_padding_bytes;
      if (satisfies_constraint_Iu08 (GEN_INT (sp_adjust))
	  && NDS32_DOUBLE_WORD_ALIGN_P (sp_adjust))
	operands[1] = GEN_INT (sp_adjust);
      else
	operands[1] = GEN_INT (0);

      /* Create assembly code pattern.  */
      snprintf (pattern, sizeof (pattern), "push25\t%%0, %%1");
    }
  else
    {
      /* For normal stack push multiple:
	 operands[0]: Rb
	 operands[1]: Re
	 operands[2]: En4 */

      /* This variable is used to check if we only need to generate En4 field.
	 As long as Rb==Re=SP_REGNUM, we set this variable to 1.  */
      int push_en4_only_p = 0;

      /* Set operands[0] and operands[1].  */
      operands[0] = gen_rtx_REG (SImode, rb_callee_saved);
      operands[1] = gen_rtx_REG (SImode, re_callee_saved);

      /* 'smw.adm $sp,[$sp],$sp,0' means push nothing.  */
      if (!cfun->machine->fp_size
	  && !cfun->machine->gp_size
	  && !cfun->machine->lp_size
	  && REGNO (operands[0]) == SP_REGNUM
	  && REGNO (operands[1]) == SP_REGNUM)
	{
	  /* No need to generate instruction.  */
	  return "";
	}
      else
	{
	  /* If Rb==Re=SP_REGNUM, we only need to generate En4 field.  */
	  if (REGNO (operands[0]) == SP_REGNUM
	      && REGNO (operands[1]) == SP_REGNUM)
	    push_en4_only_p = 1;

	  /* Create assembly code pattern.
	     We need to handle the form: "Rb, Re, { $fp $gp $lp }".  */
	  snprintf (pattern, sizeof (pattern),
		    "push.s\t%s{%s%s%s }",
		    push_en4_only_p ? "" : "%0, %1, ",
		    cfun->machine->fp_size ? " $fp" : "",
		    cfun->machine->gp_size ? " $gp" : "",
		    cfun->machine->lp_size ? " $lp" : "");
	}
    }

  /* We use output_asm_insn() to output assembly code by ourself.  */
  output_asm_insn (pattern, operands);
  return "";
}

/* Function to output stack pop operation.
   We need to deal with normal stack pop multiple or stack v3pop.  */
const char *
nds32_output_stack_pop (rtx par_rtx ATTRIBUTE_UNUSED)
{
  /* A string pattern for output_asm_insn().  */
  char pattern[100];
  /* The operands array which will be used in output_asm_insn().  */
  rtx operands[3];
  /* Pick up callee-saved first regno and last regno for further use.  */
  int rb_callee_saved = cfun->machine->callee_saved_first_gpr_regno;
  int re_callee_saved = cfun->machine->callee_saved_last_gpr_regno;

  /* If we step here, we are going to do v3pop or multiple pop operation.  */

  /* The v3push/v3pop instruction should only be applied on
     none-isr and none-variadic function.  */
  if (TARGET_V3PUSH
      && !nds32_isr_function_p (current_function_decl)
      && (cfun->machine->va_args_size == 0))
    {
      /* For stack v3pop:
	   operands[0]: Re
	   operands[1]: imm8u */

      /* This variable is to check if 'pop25 Re,imm8u' is available.  */
      int sp_adjust;

      /* Set operands[0].  */
      operands[0] = gen_rtx_REG (SImode, re_callee_saved);

      /* Check if we can generate 'pop25 Re,imm8u',
	 otherwise, generate 'pop25 Re,0'.
	 We have to consider alloca issue as well.
	 If the function does call alloca(), the stack pointer is not fixed.
	 In that case, we cannot use 'pop25 Re,imm8u' directly.
	 We have to caculate stack pointer from frame pointer
	 and then use 'pop25 Re,0'.  */
      sp_adjust = cfun->machine->local_size
		  + cfun->machine->out_args_size
		  + cfun->machine->callee_saved_area_gpr_padding_bytes;
      if (satisfies_constraint_Iu08 (GEN_INT (sp_adjust))
	  && NDS32_DOUBLE_WORD_ALIGN_P (sp_adjust)
	  && !cfun->calls_alloca)
	operands[1] = GEN_INT (sp_adjust);
      else
	operands[1] = GEN_INT (0);

      /* Create assembly code pattern.  */
      snprintf (pattern, sizeof (pattern), "pop25\t%%0, %%1");
    }
  else
    {
      /* For normal stack pop multiple:
	 operands[0]: Rb
	 operands[1]: Re
	 operands[2]: En4 */

      /* This variable is used to check if we only need to generate En4 field.
	 As long as Rb==Re=SP_REGNUM, we set this variable to 1.  */
      int pop_en4_only_p = 0;

      /* Set operands[0] and operands[1].  */
      operands[0] = gen_rtx_REG (SImode, rb_callee_saved);
      operands[1] = gen_rtx_REG (SImode, re_callee_saved);

      /* 'lmw.bim $sp,[$sp],$sp,0' means pop nothing.  */
      if (!cfun->machine->fp_size
	  && !cfun->machine->gp_size
	  && !cfun->machine->lp_size
	  && REGNO (operands[0]) == SP_REGNUM
	  && REGNO (operands[1]) == SP_REGNUM)
	{
	  /* No need to generate instruction.  */
	  return "";
	}
      else
	{
	  /* If Rb==Re=SP_REGNUM, we only need to generate En4 field.  */
	  if (REGNO (operands[0]) == SP_REGNUM
	      && REGNO (operands[1]) == SP_REGNUM)
	    pop_en4_only_p = 1;

	  /* Create assembly code pattern.
	     We need to handle the form: "Rb, Re, { $fp $gp $lp }".  */
	  snprintf (pattern, sizeof (pattern),
		    "pop.s\t%s{%s%s%s }",
		    pop_en4_only_p ? "" : "%0, %1, ",
		    cfun->machine->fp_size ? " $fp" : "",
		    cfun->machine->gp_size ? " $gp" : "",
		    cfun->machine->lp_size ? " $lp" : "");
	}
    }

  /* We use output_asm_insn() to output assembly code by ourself.  */
  output_asm_insn (pattern, operands);
  return "";
}

/* Function to generate PC relative jump table.
   Refer to nds32.md for more details.

   The following is the sample for the case that diff value
   can be presented in '.short' size.

     addi    $r1, $r1, -(case_lower_bound)
     slti    $ta, $r1, (case_number)
     beqz    $ta, .L_skip_label

     la      $ta, .L35             ! get jump table address
     lh      $r1, [$ta + $r1 << 1] ! load symbol diff from jump table entry
     addi    $ta, $r1, $ta
     jr5     $ta

     ! jump table entry
   L35:
     .short  .L25-.L35
     .short  .L26-.L35
     .short  .L27-.L35
     .short  .L28-.L35
     .short  .L29-.L35
     .short  .L30-.L35
     .short  .L31-.L35
     .short  .L32-.L35
     .short  .L33-.L35
     .short  .L34-.L35 */
const char *
nds32_output_casesi_pc_relative (rtx *operands)
{
  machine_mode mode;
  rtx diff_vec;

  diff_vec = PATTERN (NEXT_INSN (as_a <rtx_insn *> (operands[1])));

  gcc_assert (GET_CODE (diff_vec) == ADDR_DIFF_VEC);

  /* Step C: "t <-- operands[1]".  */
  output_asm_insn ("la\t$ta, %l1", operands);

  /* Get the mode of each element in the difference vector.  */
  mode = GET_MODE (diff_vec);

  /* Step D: "z <-- (mem (plus (operands[0] << m) t))",
     where m is 0, 1, or 2 to load address-diff value from table.  */
  switch (mode)
    {
    case E_QImode:
      output_asm_insn ("lb\t%2, [$ta + %0 << 0]", operands);
      break;
    case E_HImode:
      output_asm_insn ("lh\t%2, [$ta + %0 << 1]", operands);
      break;
    case E_SImode:
      output_asm_insn ("lw\t%2, [$ta + %0 << 2]", operands);
      break;
    default:
      gcc_unreachable ();
    }

  /* Step E: "t <-- z + t".
     Add table label_ref with address-diff value to
     obtain target case address.  */
  output_asm_insn ("add\t$ta, %2, $ta", operands);

  /* Step F: jump to target with register t.  */
  if (TARGET_16_BIT)
    return "jr5\t$ta";
  else
    return "jr\t$ta";
}

/* Function to generate normal jump table.  */
const char *
nds32_output_casesi (rtx *operands)
{
  /* Step C: "t <-- operands[1]".  */
  output_asm_insn ("la\t$ta, %l1", operands);

  /* Step D: "z <-- (mem (plus (operands[0] << 2) t))".  */
  output_asm_insn ("lw\t%2, [$ta + %0 << 2]", operands);

  /* No need to perform Step E, which is only used for
     pc relative jump table.  */

  /* Step F: jump to target with register z.  */
  if (TARGET_16_BIT)
    return "jr5\t%2";
  else
    return "jr\t%2";
}

/* Auxiliary functions for lwm/smw.  */
bool
nds32_valid_smw_lwm_base_p (rtx op)
{
  rtx base_addr;

  if (!MEM_P (op))
    return false;

  base_addr = XEXP (op, 0);

  if (REG_P (base_addr))
    return true;
  else
    {
      if (GET_CODE (base_addr) == POST_INC
	  && REG_P (XEXP (base_addr, 0)))
        return true;
    }

  return false;
}

/* ------------------------------------------------------------------------ */
const char *
nds32_output_smw_single_word (rtx *operands)
{
  char buff[100];
  unsigned regno;
  int enable4;
  bool update_base_p;
  rtx base_addr = operands[0];
  rtx base_reg;
  rtx otherops[2];

  if (REG_P (XEXP (base_addr, 0)))
    {
      update_base_p = false;
      base_reg = XEXP (base_addr, 0);
    }
  else
    {
      update_base_p = true;
      base_reg = XEXP (XEXP (base_addr, 0), 0);
    }

  const char *update_base = update_base_p ? "m" : "";

  regno = REGNO (operands[1]);

  otherops[0] = base_reg;
  otherops[1] = operands[1];

  if (regno >= 28)
    {
      enable4 = nds32_regno_to_enable4 (regno);
      sprintf (buff, "smw.bi%s\t$sp, [%%0], $sp, %x", update_base, enable4);
    }
  else
    {
      sprintf (buff, "smw.bi%s\t%%1, [%%0], %%1", update_base);
    }
  output_asm_insn (buff, otherops);
  return "";
}

const char *
nds32_output_lmw_single_word (rtx *operands)
{
  char buff[100];
  unsigned regno;
  bool update_base_p;
  int enable4;
  rtx base_addr = operands[1];
  rtx base_reg;
  rtx otherops[2];

  if (REG_P (XEXP (base_addr, 0)))
    {
      update_base_p = false;
      base_reg = XEXP (base_addr, 0);
    }
  else
    {
      update_base_p = true;
      base_reg = XEXP (XEXP (base_addr, 0), 0);
    }

  const char *update_base = update_base_p ? "m" : "";

  regno = REGNO (operands[0]);

  otherops[0] = operands[0];
  otherops[1] = base_reg;

  if (regno >= 28)
    {
      enable4 = nds32_regno_to_enable4 (regno);
      sprintf (buff, "lmw.bi%s\t$sp, [%%1], $sp, %x", update_base, enable4);
    }
  else
    {
      sprintf (buff, "lmw.bi%s\t%%0, [%%1], %%0", update_base);
    }
  output_asm_insn (buff, otherops);
  return "";
}

void
nds32_expand_unaligned_load (rtx *operands, enum machine_mode mode)
{
  /* Initial memory offset.  */
  int offset = WORDS_BIG_ENDIAN ? GET_MODE_SIZE (mode) - 1 : 0;
  int offset_adj = WORDS_BIG_ENDIAN ? -1 : 1;
  /* Initial register shift byte.  */
  int shift = 0;
  /* The first load byte instruction is not the same. */
  int width = GET_MODE_SIZE (mode) - 1;
  rtx mem[2];
  rtx reg[2];
  rtx sub_reg;
  rtx temp_reg, temp_sub_reg;
  int num_reg;

  /* Generating a series of load byte instructions.
     The first load byte instructions and other
     load byte instructions are not the same. like:
     First:
       lbi reg0, [mem]
       zeh reg0, reg0
     Second:
       lbi temp_reg, [mem + offset]
       sll temp_reg, (8 * shift)
       ior reg0, temp_reg

       lbi temp_reg, [mem + (offset + 1)]
       sll temp_reg, (8 * (shift + 1))
       ior reg0, temp_reg  */

  temp_reg = gen_reg_rtx (SImode);
  temp_sub_reg = gen_lowpart (QImode, temp_reg);

  if (mode == DImode)
    {
      /* Load doubleword, we need two registers to access.  */
      reg[0] = simplify_gen_subreg (SImode, operands[0],
				    GET_MODE (operands[0]), 0);
      reg[1] = simplify_gen_subreg (SImode, operands[0],
				    GET_MODE (operands[0]), 4);
      /* A register only store 4 byte.  */
      width = GET_MODE_SIZE (SImode) - 1;
    }
  else
    {
      reg[0] = operands[0];
    }

  for (num_reg = (mode == DImode) ? 2 : 1; num_reg > 0; num_reg--)
    {
      sub_reg = gen_lowpart (QImode, reg[0]);
      mem[0] = gen_rtx_MEM (QImode, plus_constant (Pmode, operands[1], offset));

      /* Generating the first part instructions.
	   lbi reg0, [mem]
	   zeh reg0, reg0 */
      emit_move_insn (sub_reg, mem[0]);
      emit_insn (gen_zero_extendqisi2 (reg[0], sub_reg));

      while (width > 0)
	{
	  offset = offset + offset_adj;
	  shift++;
	  width--;

	  mem[1] = gen_rtx_MEM (QImode, plus_constant (Pmode,
						       operands[1],
						       offset));
	  /* Generating the second part instructions.
	       lbi temp_reg, [mem + offset]
	       sll temp_reg, (8 * shift)
	       ior reg0, temp_reg  */
	  emit_move_insn (temp_sub_reg, mem[1]);
	  emit_insn (gen_ashlsi3 (temp_reg, temp_reg,
				  GEN_INT (shift * 8)));
	  emit_insn (gen_iorsi3 (reg[0], reg[0], temp_reg));
	}

      if (mode == DImode)
	{
	  /* Using the second register to load memory information. */
	  reg[0] = reg[1];
	  shift = 0;
	  width = GET_MODE_SIZE (SImode) - 1;
	  offset = offset + offset_adj;
	}
    }
}

void
nds32_expand_unaligned_store (rtx *operands, enum machine_mode mode)
{
  /* Initial memory offset.  */
  int offset = WORDS_BIG_ENDIAN ? GET_MODE_SIZE (mode) - 1 : 0;
  int offset_adj = WORDS_BIG_ENDIAN ? -1 : 1;
  /* Initial register shift byte.  */
  int shift = 0;
  /* The first load byte instruction is not the same. */
  int width = GET_MODE_SIZE (mode) - 1;
  rtx mem[2];
  rtx reg[2];
  rtx sub_reg;
  rtx temp_reg, temp_sub_reg;
  int num_reg;

  /* Generating a series of store byte instructions.
     The first store byte instructions and other
     load byte instructions are not the same. like:
     First:
	sbi  reg0, [mem + 0]
     Second:
	srli    temp_reg, reg0, (8 * shift)
	sbi	temp_reg, [mem + offset]  */

  temp_reg = gen_reg_rtx (SImode);
  temp_sub_reg = gen_lowpart (QImode, temp_reg);

  if (mode == DImode)
    {
      /* Load doubleword, we need two registers to access.  */
      reg[0] = simplify_gen_subreg (SImode, operands[1],
				    GET_MODE (operands[1]), 0);
      reg[1] = simplify_gen_subreg (SImode, operands[1],
				    GET_MODE (operands[1]), 4);
      /* A register only store 4 byte.  */
      width = GET_MODE_SIZE (SImode) - 1;
    }
  else
    {
      reg[0] = operands[1];
    }

  for (num_reg = (mode == DImode) ? 2 : 1; num_reg > 0; num_reg--)
    {
      sub_reg = gen_lowpart (QImode, reg[0]);
      mem[0] = gen_rtx_MEM (QImode, plus_constant (Pmode, operands[0], offset));

      /* Generating the first part instructions.
	   sbi reg0, [mem + 0] */
      emit_move_insn (mem[0], sub_reg);

      while (width > 0)
	{
	  offset = offset + offset_adj;
	  shift++;
	  width--;

	  mem[1] = gen_rtx_MEM (QImode, plus_constant (Pmode,
						       operands[0],
						       offset));
	  /* Generating the second part instructions.
	       srli  temp_reg, reg0, (8 * shift)
	       sbi   temp_reg, [mem + offset]  */
	  emit_insn (gen_lshrsi3 (temp_reg, reg[0],
				  GEN_INT (shift * 8)));
	  emit_move_insn (mem[1], temp_sub_reg);
	}

      if (mode == DImode)
	{
	  /* Using the second register to load memory information. */
	  reg[0] = reg[1];
	  shift = 0;
	  width = GET_MODE_SIZE (SImode) - 1;
	  offset = offset + offset_adj;
	}
    }
}

const char *
nds32_output_cbranchsi4_equality_zero (rtx_insn *insn, rtx *operands)
{
  enum rtx_code code;
  bool long_jump_p = false;

  code = GET_CODE (operands[0]);

  /* This zero-comparison conditional branch has two forms:
       32-bit instruction =>          beqz/bnez           imm16s << 1
       16-bit instruction => beqzs8/bnezs8/beqz38/bnez38  imm8s << 1

     For 32-bit case,
     we assume it is always reachable. (but check range -65500 ~ 65500)

     For 16-bit case,
     it must satisfy { 255 >= (label - pc) >= -256 } condition.
     However, since the $pc for nds32 is at the beginning of the instruction,
     we should leave some length space for current insn.
     So we use range -250 ~ 250.  */

  switch (get_attr_length (insn))
    {
    case 8:
      long_jump_p = true;
      /* fall through  */
    case 2:
      if (which_alternative == 0)
	{
	  /* constraint: t */
	  /*    b<cond>zs8  .L0
	      or
		b<inverse_cond>zs8  .LCB0
		j  .L0
	      .LCB0:
	   */
	  output_cond_branch_compare_zero (code, "s8", long_jump_p,
					   operands, true);
	  return "";
	}
      else if (which_alternative == 1)
	{
	  /* constraint: l */
	  /*    b<cond>z38  $r0, .L0
	      or
		b<inverse_cond>z38  $r0, .LCB0
		j  .L0
	      .LCB0:
	   */
	  output_cond_branch_compare_zero (code, "38", long_jump_p,
					   operands, false);
	  return "";
	}
      else
	{
	  /* constraint: r */
	  /* For which_alternative==2, it should not be here.  */
	  gcc_unreachable ();
	}
    case 10:
      /* including constraints: t, l, and r */
      long_jump_p = true;
      /* fall through  */
    case 4:
      /* including constraints: t, l, and r */
      output_cond_branch_compare_zero (code, "", long_jump_p, operands, false);
      return "";

    default:
      gcc_unreachable ();
    }
}

const char *
nds32_output_cbranchsi4_equality_reg (rtx_insn *insn, rtx *operands)
{
  enum rtx_code code;
  bool long_jump_p, r5_p;
  int insn_length;

  insn_length = get_attr_length (insn);

  long_jump_p = (insn_length == 10 || insn_length == 8) ? true : false;
  r5_p = (insn_length == 2 || insn_length == 8) ? true : false;

  code = GET_CODE (operands[0]);

  /* This register-comparison conditional branch has one form:
       32-bit instruction =>          beq/bne           imm14s << 1

     For 32-bit case,
     we assume it is always reachable. (but check range -16350 ~ 16350).  */

  switch (code)
    {
    case EQ:
    case NE:
      output_cond_branch (code, "", r5_p, long_jump_p, operands);
      return "";

    default:
      gcc_unreachable ();
    }
}

const char *
nds32_output_cbranchsi4_equality_reg_or_const_int (rtx_insn *insn,
						   rtx *operands)
{
  enum rtx_code code;
  bool long_jump_p, r5_p;
  int insn_length;

  insn_length = get_attr_length (insn);

  long_jump_p = (insn_length == 10 || insn_length == 8) ? true : false;
  r5_p = (insn_length == 2 || insn_length == 8) ? true : false;

  code = GET_CODE (operands[0]);

  /* This register-comparison conditional branch has one form:
       32-bit instruction =>          beq/bne           imm14s << 1
       32-bit instruction =>         beqc/bnec          imm8s << 1

     For 32-bit case, we assume it is always reachable.
     (but check range -16350 ~ 16350 and -250 ~ 250).  */

  switch (code)
    {
    case EQ:
    case NE:
      if (which_alternative == 2)
	{
	  /* r, Is11 */
	  /* b<cond>c */
	  output_cond_branch (code, "c", r5_p, long_jump_p, operands);
	}
      else
	{
	  /* r, r */
	  /* v, r */
	  output_cond_branch (code, "", r5_p, long_jump_p, operands);
	}
      return "";
    default:
      gcc_unreachable ();
    }
}

const char *
nds32_output_cbranchsi4_greater_less_zero (rtx_insn *insn, rtx *operands)
{
  enum rtx_code code;
  bool long_jump_p;
  int insn_length;

  insn_length = get_attr_length (insn);

  gcc_assert (insn_length == 4 || insn_length == 10);

  long_jump_p = (insn_length == 10) ? true : false;

  code = GET_CODE (operands[0]);

  /* This zero-greater-less-comparison conditional branch has one form:
       32-bit instruction =>      bgtz/bgez/bltz/blez     imm16s << 1

     For 32-bit case, we assume it is always reachable.
     (but check range -65500 ~ 65500).  */

  switch (code)
    {
    case GT:
    case GE:
    case LT:
    case LE:
      output_cond_branch_compare_zero (code, "", long_jump_p, operands, false);
      break;
    default:
      gcc_unreachable ();
    }
  return "";
}

/* Return true X is need use long call.  */
bool
nds32_long_call_p (rtx symbol)
{
  return TARGET_CMODEL_LARGE;
}
