class alignedImage {
    public:
        alignedImage(unsigned int tid);
        ~alignedImage();

        void align(double x, double y);
        void alignAdd(double x, double y);
        void draw();
    private:
        unsigned int textureID;
        double xAlign;
        double yAlign;
};