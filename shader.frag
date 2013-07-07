uniform float time; 
uniform vec2 mouse; 
uniform vec2 resolution; 
uniform sampler2D tex; 
 
void main( void ) 
{ 
    vec2 p = gl_FragCoord.xy / resolution.xy * 2.0 - 0.5; 
     
    vec2 uPos = ( gl_FragCoord.xy / resolution.xy );//normalize wrt y axis 
    uPos *= 3.0; 
    uPos = abs(uPos); 
    uPos -= vec2((resolution.x/resolution.y)/1.05, 0.0);//shift origin to center 
    //uPos.x -= 1.5; 
    uPos.y -= 2.0; 
     
    vec3 color = vec3(0.0); 
    float vertColor = 0.0; 
    for( float i = 0.0; i < 10.0; ++i ) 
    { 
        float t = time * (1.5); 
     
        uPos.y += sin( uPos.x*(i+5.0) + t+i/5.0 ) * 0.1; 
        float fTemp = abs(2.4 / uPos.y / 180.0); 
        vertColor += fTemp; 
        color += vec3( fTemp, fTemp*i/15.0, pow(fTemp,0.99)*3.0 ); 
    } 
     
    vec4 color_final = vec4(color, 0); 
    
    /*
    vec2 uv = gl_FragCoord.xy / resolution.xy; 
    vec4 s = vec4(0.0); 
    float o = 0.1; 
    s += texture2D(tex, uv + vec2( -o, -o)); 
    s += texture2D(tex, uv + vec2(10.0, -o)); 
    s += texture2D(tex, uv + vec2(  o, -o)); 
    s += texture2D(tex, uv + vec2( -o, 10.0)); 
    s += texture2D(tex, uv + vec2(10.0, 10.0)); 
    s += texture2D(tex, uv + vec2(  o, 10.0)); 
    s += texture2D(tex, uv + vec2( -o,  o)); 
    s += texture2D(tex, uv + vec2(10.0,  o)); 
    s += texture2D(tex, uv + vec2(  o,  o)); 
    s /= 90.0; 
          */
    vec4 s = vec4(0.0); 
    color_final = mix(color_final, s, sin(p.x*p.y/ + time*8.0) * 0.15 + 0.75 );     

    float avg = color_final.r + color_final.g * color_final.b; 
    avg /= 1.0; 
    color_final = vec4(avg, avg, color_final.b, avg); 
     
    gl_FragColor = color_final; 
 
}