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
    vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vVertex);
    float diffuse = max(dot(vNormal, lightDir), 0.0)*0.8;
    gl_FragColor = vec4(vec3(0.2 + diffuse), 1.0)*vColor;
}