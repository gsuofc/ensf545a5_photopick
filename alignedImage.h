#ifndef ALIGNEDIMAGE_H
#define ALIGNEDIMAGE_H


class alignedImage {
    public:
        alignedImage(unsigned int tid);
        ~alignedImage();

        void align(double x, double y);
        void alignAdd(double x, double y);
        void draw(float opacity=1.0);
        double getXAlign() { return xAlign; };
        double getYAlign() { return yAlign; };
		double getRotationAngle() { return rotationAngle; };
    private:
        unsigned int textureID;
        double xAlign;
        double yAlign;
        double rotationAngle; 
};

#endif