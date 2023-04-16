# SPDX-License-Identifier: Unlicense
project(
    'libtomfloat',
    'c',
    license: 'Unlicense',
    version: '0.02',
    default_options: [
        'c_std=c99',
        'warning_level=3',
        # libtomfloat relies on dead code optimization to remove paths
        # not compiled on different platforms.
        'optimization=1',
    ],
)

srcs = files(
    'libtommath-1.2.0/tommath.c',
    'mpf_abs.c',
    'mpf_acos.c',
    'mpf_add.c',
    'mpf_add_d.c',
    'mpf_asin.c',
    'mpf_atan.c',
    'mpf_clear.c',
    'mpf_clear_multi.c',
    'mpf_cmp.c',
    'mpf_cmp_d.c',
    'mpf_const_0.c',
    'mpf_const_1pi.c',
    'mpf_const_1r2.c',
    'mpf_const_2pi.c',
    'mpf_const_2rpi.c',
    'mpf_const_d.c',
    'mpf_const_e.c',
    'mpf_const_l10e.c',
    'mpf_const_l2e.c',
    'mpf_const_le2.c',
    'mpf_const_ln_d.c',
    'mpf_const_pi.c',
    'mpf_const_pi2.c',
    'mpf_const_pi4.c',
    'mpf_const_r2.c',
    'mpf_const_sqrt_d.c',
    'mpf_copy.c',
    'mpf_cos.c',
    'mpf_div.c',
    'mpf_div_2.c',
    'mpf_div_d.c',
    'mpf_exch.c',
    'mpf_exp.c',
    'mpf_init.c',
    'mpf_init_copy.c',
    'mpf_init_multi.c',
    'mpf_inv.c',
    'mpf_invsqrt.c',
    'mpf_iterations.c',
    'mpf_ln.c',
    'mpf_mul.c',
    'mpf_mul_2.c',
    'mpf_mul_d.c',
    'mpf_neg.c',
    'mpf_normalize.c',
    'mpf_normalize_to.c',
    'mpf_pow.c',
    'mpf_sin.c',
    'mpf_sqr.c',
    'mpf_sqrt.c',
    'mpf_sub.c',
    'mpf_sub_d.c',
    'mpf_tan.c',
)

incdirs = include_directories(
    '.',
    'libtommath-1.2.0',
)

install_headers(
    'tomfloat.h',
)

amalgamate_exe = executable(
    'amalgamate',
    files('tools/amalgamate.c'),
)

tomfloat_srcs = custom_target(
    'tomfloat.c and tomfloat.h',
    input: srcs,
    output: ['tomfloat.c', 'tomfloat.h'],
    command: [amalgamate_exe, meson.source_root()],
)

subdir('demos')
