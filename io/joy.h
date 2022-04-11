class joystick{
    private:
        SDL_GameController *joy;
        uint8_t joy_id;
        bool init;
        bool check_quit(void);
    public:
        joystick(uint8_t index);
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool start = false;         // Button ID 6
        bool select = false;        // Button ID 7
        bool a = false;             // Button ID 0
        bool b = false;             // Button ID 1
        bool x = false;             // Button ID 2
        bool y = false;             // Button ID 3
        bool l1 = false;            // Button ID 4
        bool l2 = false;            // Button ID 10
        bool l3 = false;            // Button ID 8
        bool r1 = false;            // Button ID 5
        bool r2 = false;            // Button ID 11
        bool r3 = false;            // Button ID 9
        bool quit = false;
        bool any = false;
        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;  
        void poll(void);
        void kill(void);
};

joystick::joystick(uint8_t index){
	init = false;

    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0 ){
        printf("SDL Joystick/gamepad init failed");
        exit(2);    
    }
    
    int mappings = SDL_GameControllerAddMappingsFromFile("gamecontrollerdb.txt");
    if (mappings == -1){
    	printf("Wasn't able to load any mappings from `gamecontrollerdb.txt` did you include it?\n");
    	exit(3);
    }
    
    joy = SDL_GameControllerOpen(index); 
    
    if (joy) {
    	init = true;
    } 
}

void joystick::poll(void){
	SDL_GameControllerUpdate();
	b 		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_B);
	a 		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_A);
	x 		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_X);
	y 		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_Y);
	
	start	= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_START);
	select	= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_BACK);
	
	l1		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	r1		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	l3		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_LEFTSTICK);
	r3		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	
	up		= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_DPAD_UP);
	down	= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	left	= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	right	= SDL_GameControllerGetButton(joy, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	
	
	
	x1 = -SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_LEFTX);
	y1 = -SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_LEFTY);
	
	x2 = -SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_RIGHTX);
	y2 = -SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_RIGHTY);
	
	l2 = (SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 1000);
	r2 = (SDL_GameControllerGetAxis(joy,SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 1000);
	
	quit = check_quit();
	
	any = (a + b + x + y + start + select + l1 + l2 + l3 + r1 + r2 + r3 + up + down + left + right) >= 1;

}

bool joystick::check_quit(void){
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0){
		if (e.type == SDL_QUIT) {
		    return true;
		    break;
		}
	}
	return false;
}

void joystick::kill(void){
	if (init){
		SDL_GameControllerClose(joy);
	}

}
