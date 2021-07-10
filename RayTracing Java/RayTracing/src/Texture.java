import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;

public class Texture {

	BufferedImage image;
	int width, height;

	public Texture(String name) {
		try {
			image = ImageIO.read(new File(name));
			width = image.getWidth();
			height = image.getHeight();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
	}

	public Color lookup(double u, double v) {
		if (0.0 <= u && u <= 1.0 && 0.0 <= v && v <= 1.0) {
			return new Color(
					image.getRGB((int) Math.floor(u * (width - 1)), height - ((int) Math.floor(v * (height - 1))) - 1));
		} else {
			return Color.BLACK;
		}
	}
}