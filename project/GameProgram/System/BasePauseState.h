#pragma once

class PauseScreen;

class BasePauseState {
public:
	virtual void Update(PauseScreen& pauseScreen) = 0;
	virtual void Draw(PauseScreen& pauseScreen) = 0;
};

class PauseMove : public BasePauseState {
public:
	void Update(PauseScreen& pauseScreen) override;
	void Draw(PauseScreen& pauseScreen) override;
};

class PauseSelectGuide : public BasePauseState {
public:
	void Update(PauseScreen& pauseScreen) override;
	void Draw(PauseScreen& pauseScreen) override;
};

class PauseSelectSceneChange : public BasePauseState {
public:
	void Update(PauseScreen& pauseScreen) override;
	void Draw(PauseScreen& pauseScreen) override;
};