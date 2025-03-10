;; Constant defintions of Andes NDS32 cpu for GNU compiler
;; Copyright (C) 2012-2018 Free Software Foundation, Inc.
;; Contributed by Andes Technology Corporation.
;;
;; This file is part of GCC.
;;
;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.
;;
;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.


;; Register numbers.
(define_constants
  [(R8_REGNUM  8)
   (TA_REGNUM 15)
   (FP_REGNUM 28)
   (GP_REGNUM 29)
   (LP_REGNUM 30)
   (SP_REGNUM 31)
  ])


;; The unpec operation index.
(define_c_enum "unspec_element" [
  UNSPEC_FFB
  UNSPEC_FFMISM
  UNSPEC_FLMISM
  UNSPEC_UALOAD_HW
  UNSPEC_UALOAD_W
  UNSPEC_UALOAD_DW
  UNSPEC_UASTORE_HW
  UNSPEC_UASTORE_W
  UNSPEC_UASTORE_DW
])

;; The unspec_volatile operation index.
(define_c_enum "unspec_volatile_element" [
  UNSPEC_VOLATILE_ISYNC
  UNSPEC_VOLATILE_ISB
  UNSPEC_VOLATILE_MFSR
  UNSPEC_VOLATILE_MFUSR
  UNSPEC_VOLATILE_MTSR
  UNSPEC_VOLATILE_MTUSR
  UNSPEC_VOLATILE_SETGIE_EN
  UNSPEC_VOLATILE_SETGIE_DIS

  UNSPEC_VOLATILE_RELAX_GROUP
  UNSPEC_VOLATILE_POP25_RETURN
])

;; ------------------------------------------------------------------------
