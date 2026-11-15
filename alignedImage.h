class alignedImage {
    public:
        alignedImage();
        ~alignedImage();

        void align(double x, double y);
        void alignAdd(double x, double y);
        void draw();
    private:
        double xAlign;
        double yAlign;
};