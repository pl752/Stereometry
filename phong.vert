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

#version 110

varying vec3 vVertex;
varying vec3 vNormal;
varying vec4 vColor;

void main() {
    gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
    vVertex = gl_Vertex.xyz;
    vNormal = gl_Normal;
    vColor = gl_Color;
}