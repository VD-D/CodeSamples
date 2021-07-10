import java.awt.Color;

public class Intersect {
     double distance;
     P3D hitPosition;
     P3D hitNormal;
     Item item;
     Color color;
     public Intersect(double d, P3D p, P3D n, Item i, Color c) {
    	 distance = d;
    	 hitPosition = p;
    	 hitNormal = n;
    	 item = i;
    	 color = c;
     }
}
