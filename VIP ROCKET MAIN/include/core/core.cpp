#ifndef Core_H
#define Core_H

class Context;

class State {
    protected:
        Context *_context;
    public:
        virtual ~State() {}

        virtual void start();

        void setContext(Context *context) {
            this->_context = context;
        }

        virtual void HandleNextPhase() = 0;
};

class Context {
 
    private:
        State *state_;

    public:
        Context(State *state) : state_(nullptr) {
            this->TransitionTo(state);
        }
        ~Context() {
            delete state_;
        }
  
        void TransitionTo(State *state) {
            if (this->state_ != nullptr)
                delete this->state_;
            this->state_ = state;
            this->state_->setContext(this);
        }

        void Start() {
            this->state_->start();
        }
         
        void RequestNextPhase() {
            this->state_->HandleNextPhase();
        }
};

#endif