public class Light {
    P3D position;
    double is;
    double id;
    double ia;
    public Light(P3D pos, double specular, double diffuse, double ambient) {
        position = pos;
        is = specular;
        id = diffuse;
        ia = ambient;
    }
}
