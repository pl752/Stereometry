/********************************************************************************
 * Copyright (c) 2024 Pleshkov Maksim
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 ********************************************************************************/
 
 constexpr char phong_vert[] = {
  0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x31,
  0x31, 0x30, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x61, 0x72, 0x79,
  0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x76,
  0x56, 0x65, 0x72, 0x74, 0x65, 0x78, 0x3b, 0x0d, 0x0a, 0x76,
  0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x76, 0x65, 0x63,
  0x33, 0x20, 0x76, 0x4e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b,
  0x0d, 0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20,
  0x76, 0x65, 0x63, 0x34, 0x20, 0x76, 0x43, 0x6f, 0x6c, 0x6f,
  0x72, 0x3b, 0x0d, 0x0a, 0x0d, 0x0a, 0x76, 0x6f, 0x69, 0x64,
  0x20, 0x6d, 0x61, 0x69, 0x6e, 0x28, 0x29, 0x20, 0x7b, 0x0d,
  0x0a, 0x20, 0x20, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x50, 0x6f,
  0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x3d, 0x20, 0x67,
  0x6c, 0x5f, 0x4d, 0x6f, 0x64, 0x65, 0x6c, 0x56, 0x69, 0x65,
  0x77, 0x50, 0x72, 0x6f, 0x6a, 0x65, 0x63, 0x74, 0x69, 0x6f,
  0x6e, 0x4d, 0x61, 0x74, 0x72, 0x69, 0x78, 0x2a, 0x67, 0x6c,
  0x5f, 0x56, 0x65, 0x72, 0x74, 0x65, 0x78, 0x3b, 0x0d, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x76, 0x56, 0x65, 0x72, 0x74, 0x65,
  0x78, 0x20, 0x3d, 0x20, 0x67, 0x6c, 0x5f, 0x56, 0x65, 0x72,
  0x74, 0x65, 0x78, 0x2e, 0x78, 0x79, 0x7a, 0x3b, 0x0d, 0x0a,
  0x20, 0x20, 0x20, 0x20, 0x76, 0x4e, 0x6f, 0x72, 0x6d, 0x61,
  0x6c, 0x20, 0x3d, 0x20, 0x67, 0x6c, 0x5f, 0x4e, 0x6f, 0x72,
  0x6d, 0x61, 0x6c, 0x3b, 0x0d, 0x0a, 0x20, 0x20, 0x20, 0x20,
  0x76, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x67,
  0x6c, 0x5f, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0d, 0x0a,
  0x7d
};
constexpr int phong_vert_len = 241;