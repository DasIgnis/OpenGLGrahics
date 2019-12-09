uniform vec4 color1, color2;
uniform int width;
uniform bool dir_x;
void main() {
	int coord = dir_x ? gl_FragCoord.x : gl_FragCoord.y;
	gl_FragColor = color2;
	if ((coord / width) % 2 == 0)
			gl_FragColor = color1;
	//if(((coord / width) % 2) == 0)
	//	gl_FragColor = color1;
}
