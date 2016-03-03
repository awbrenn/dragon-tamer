varying vec3 ec_vnormal, ec_vposition;

void lightGeometry(int light_index,float influence){
    vec3 P, N, L, V, H, R;
    vec4 diffuse_color = gl_FrontMaterial.diffuse;
    vec4 specular_color = gl_FrontMaterial.specular;
    const float PI = 3.141592653589793238462643383;
    float shininess = gl_FrontMaterial.shininess;

    P = ec_vposition;
    N = normalize(ec_vnormal);
    L = normalize(gl_LightSource[light_index].position);
    V = normalize(-P);
    H = normalize(L+V);

    diffuse_color *= max(dot(N,L),0.0);
    specular_color *= (shininess + 2) / (8 * PI) * pow(max(dot(H,N),0.0),shininess);
    gl_FragColor += (diffuse_color + specular_color) * influence;
}

void capFragColor() {
    gl_FragColor.x = min(gl_FragColor.x, 1.0);
    gl_FragColor.y = min(gl_FragColor.y, 1.0);
    gl_FragColor.z = min(gl_FragColor.z, 1.0);
}

void main() {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);

   lightGeometry(0,1.0);
   lightGeometry(1,0.3);
   lightGeometry(2,1.0);
}
