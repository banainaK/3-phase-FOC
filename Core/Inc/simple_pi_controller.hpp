class SimplePIController {
    public: 
        SimplePIController(float kp, float ki, float reference);
        void update(float actual, float dt);
    
    float kp; 
    float ki; 
    float reference;

    float output;
    float integral;
};