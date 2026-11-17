class alignedImage {
    public:
        alignedImage(unsigned int tid);
        ~alignedImage();

        void align(double x, double y);
        void alignAdd(double x, double y);
        void draw(float opacity=1.0);
    private:
        unsigned int textureID;
        double xAlign;
        double yAlign;
};