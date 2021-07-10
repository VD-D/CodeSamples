import java.awt.Color;
import java.util.ArrayList;

public class Scene extends ArrayList<Item> {

	Color background = Color.black;
	Light myLight = new Light(new P3D(45.0,80.0,-35.0), 0.3, 2.0, 0.9);

	public Color raytrace(Ray r) {
		
		Double mindis = null;
		Intersect intersect = null;
		
		for (Item s : this) {
			Intersect i = s.intersect(r);
			if (i != null) {
				if (intersect == null || i.distance < mindis) {
					mindis = i.distance;
					intersect = i;
				}
			}
		}
		if (intersect != null) {
		    double ip = phongIllumination(1, 1, 1, 1, myLight, intersect, r);
		    Color icolor = intersect.color;
		    Color lightcolor = Color.white;
			return blend(icolor, lightcolor, ip);
		} else {
			return background;
		}
	}
	
	private double clamp(double r) {
		return (r < 0.0 ? 0.0 : (r > 1.0 ? 1.0 : r));
	}

	private double dot(P3D a, P3D b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }

    private double phongIllumination(double ks, double kd, double ka, double alpha, Light light, Intersect intersect,
                                     Ray ray) {
        ks = clamp(ks);
        kd = clamp(kd);
        ka = clamp(ka);
        double ia = light.ia;
        double id = light.id;
        double is = light.is;
        P3D lightPos = light.position;

        double dx = intersect.hitPosition.x - lightPos.x;
        double dy = intersect.hitPosition.y - lightPos.y;
        double dz = intersect.hitPosition.z - lightPos.z;

        P3D l = new P3D(dx,dy,dz);
        l = l.normalize();
        P3D r = l.reflect(lightPos).normalize();

	    return -((ka * ia) + (kd * dot(l, intersect.hitNormal.normalize()) * id + ks * Math.pow(dot(r, ray.direction.normalize()), alpha) * is));
    }


    private Color blend(Color c1, Color c2, double ratio) {
        if ( ratio > 1.0 ) ratio = 1.0;
        else if ( ratio < 0.0 ) ratio = 0.0;
        double iRatio = 1.0 - ratio;

        int i1 = c1.getRGB();
        int i2 = c2.getRGB();

        int a1 = (i1 >> 24 & 0xff);
        int r1 = ((i1 & 0xff0000) >> 16);
        int g1 = ((i1 & 0xff00) >> 8);
        int b1 = (i1 & 0xff);

        int a2 = (i2 >> 24 & 0xff);
        int r2 = ((i2 & 0xff0000) >> 16);
        int g2 = ((i2 & 0xff00) >> 8);
        int b2 = (i2 & 0xff);

        int a = (int)((a1 * iRatio) + (a2 * ratio));
        int r = (int)((r1 * iRatio) + (r2 * ratio));
        int g = (int)((g1 * iRatio) + (g2 * ratio));
        int b = (int)((b1 * iRatio) + (b2 * ratio));

        return new Color( a << 24 | r << 16 | g << 8 | b );
    }
}
