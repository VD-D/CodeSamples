public class View {
	P3D camera;
	P3D direction;
	P3D down;
	P3D across;

	public View(P3D camera, P3D direction, P3D across, P3D down) {
		super();
		this.camera = camera;
		this.direction = direction;
		this.down = down;
		this.across = across;
	}
}
