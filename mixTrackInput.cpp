//THIS FILE ONLY CONTAINS CODE WHICH SHOULD BE IMPLEMENTED IN THE CLASS WHICH CONTROLS THE MOTORS OF THE TRACKS
//THEREFORE, THIS FILE IS NOT SUPPOSED TO WORK ON ITS OWN, AND IT SHOULD BE DELETED ONCE THE CODE IS IMPLEMENTED INTO THE RIGHT FILE

struct TrackSpeed {
	float leftMotor;
	float rightMotor;
};
TrackSpeed mixTrackInput(float throttle, float direction) {
	// Calculate percentage of throttle versus direction
	float total = positive(throttle) + positive(direction);
	throttle = throttle / total;
	direction = direction / total;

	float leftMotor = throttle + direction;
	float rightMotor = throttle - direction;

	return { leftMotor, rightMotor };
}

template <typename T>
T positive(T x) {
	return x < 0 ? x*-1 : x;
}
