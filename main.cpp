#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <cctype>

using namespace std;

// ---------------- GLOBAL VARIABLES ----------------
vector<string> words;

string currentWord = "";
string scrambledWord = "";
string userInput = "";

int currentWordIndex = 0;
int currentLevel = 1;
int score = 0;
int solvedWordsInLevel = 0;

const int wordsPerLevel = 5;
const float timePerWord = 10.0f;

bool isTyping = false;
bool answerCorrect = false;
bool levelCompleted = false;
bool gameCompleted = false;
bool levelFailed = false;

// ---------------- FUNCTION: LOAD WORDS ----------------
void loadWordsFromFile() {
    ifstream file("words.txt");
    string word;

    if (!file.is_open()) {
        cout << "words.txt file open nahi hui.\n";
        return;
    }

    while (file >> word) {
        string cleanWord = "";

        for (int i = 0; i < (int)word.length(); i++) {
            if (isalpha((unsigned char)word[i])) {
                cleanWord += tolower(word[i]);
            }
        }

        if (cleanWord.length() >= 3) {
            words.push_back(cleanWord);
        }
    }

    file.close();
}

// ---------------- FUNCTION: SCRAMBLE WORD ----------------
string makeScrambledWord(string word, mt19937& rng) {
    string temp = word;

    if (word.length() <= 1) {
        return word;
    }

    do {
        shuffle(temp.begin(), temp.end(), rng);
    } while (temp == word);

    return temp;
}

// ---------------- FUNCTION: LOAD CURRENT WORD ----------------
void loadCurrentWord(mt19937& rng) {
    currentWord = words[currentWordIndex];
    scrambledWord = makeScrambledWord(currentWord, rng);

    userInput = "";
    isTyping = false;
    answerCorrect = false;
}

// ---------------- FUNCTION: CHECK LEVEL COMPLETE ----------------
bool checkLevelComplete() {
    if (solvedWordsInLevel == wordsPerLevel) {
        return true;
    }
    return false;
}

// ---------------- FUNCTION: GET LEVEL START INDEX ----------------
int getLevelStartIndex(int levelNumber) {
    return (levelNumber - 1) * wordsPerLevel;
}

int main() {
    // ---------------- LOAD WORDS ----------------
    loadWordsFromFile();

    if (words.size() == 0) {
        cout << "No words loaded from words.txt\n";
        return 1;
    }

    random_device rd;
    mt19937 rng(rd());

    shuffle(words.begin(), words.end(), rng);

    // ---------------- WINDOW ----------------
    sf::RenderWindow window(sf::VideoMode({1100, 720}), "Word Scramble Game");
    window.setFramerateLimit(60);

    // ---------------- FONT ----------------
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        cout << "Font load nahi hua.\n";
        return 1;
    }

    // ---------------- LOAD FIRST WORD ----------------
    loadCurrentWord(rng);

    // ---------------- TIMER ----------------
    sf::Clock wordClock;

    // ---------------- COLORS ----------------
    sf::Color bgColor(20, 28, 45);
    sf::Color topPanelColor(255, 255, 255);      // white title box
    sf::Color wordCardColor(47, 58, 95);
    sf::Color accentColor(90, 180, 255);
    sf::Color submitColor(70, 130, 255);
    sf::Color submitHoverColor(100, 160, 255);
    sf::Color nextWordColor(255, 150, 70);
    sf::Color nextWordHoverColor(255, 180, 100);
    sf::Color nextLevelColor(60, 190, 110);
    sf::Color nextLevelHoverColor(90, 220, 140);
    sf::Color reattemptColor(220, 80, 80);
    sf::Color reattemptHoverColor(245, 110, 110);
    sf::Color errorColor(255, 80, 80);

    // ---------------- TOP PANEL ----------------
    sf::RectangleShape topPanel({1020.f, 88.f});
    topPanel.setPosition({40.f, 20.f});
    topPanel.setFillColor(topPanelColor);

    // ---------------- TITLE ----------------
    sf::Text title(font, "Word Scramble Game", 38);
    title.setPosition({330.f, 40.f});
    title.setFillColor(sf::Color::Black);   // black title

    // ---------------- TOP TEXTS ----------------
    sf::Text levelText(font, "", 26);
    levelText.setPosition({70.f, 135.f});
    levelText.setFillColor(sf::Color::White);

    sf::Text scoreText(font, "", 26);
    scoreText.setPosition({900.f, 135.f});
    scoreText.setFillColor(sf::Color::White);

    sf::Text progressText(font, "", 24);
    progressText.setPosition({430.f, 135.f});
    progressText.setFillColor(sf::Color::White);

    // ---------------- SMALL WATCH ICON ----------------
    sf::CircleShape watchCircle(14.f);
    watchCircle.setPosition({72.f, 182.f});
    watchCircle.setFillColor(sf::Color::Transparent);
    watchCircle.setOutlineThickness(3.f);
    watchCircle.setOutlineColor(sf::Color::White);

    sf::RectangleShape watchTop({6.f, 5.f});
    watchTop.setPosition({84.f, 176.f});
    watchTop.setFillColor(sf::Color::White);

    sf::RectangleShape watchHand1({2.f, 9.f});
    watchHand1.setPosition({85.f, 188.f});
    watchHand1.setFillColor(sf::Color::White);

    sf::RectangleShape watchHand2({8.f, 2.f});
    watchHand2.setPosition({85.f, 195.f});
    watchHand2.setFillColor(sf::Color::White);

    sf::Text timerText(font, "", 22);
    timerText.setPosition({110.f, 182.f});
    timerText.setFillColor(sf::Color::White);

    // ---------------- PROGRESS BAR ----------------
    sf::RectangleShape progressBarBack({320.f, 18.f});
    progressBarBack.setPosition({390.f, 175.f});
    progressBarBack.setFillColor(sf::Color(60, 65, 75));

    sf::RectangleShape progressBarFill({0.f, 18.f});
    progressBarFill.setPosition({390.f, 175.f});
    progressBarFill.setFillColor(accentColor);

    // ---------------- INSTRUCTION ----------------
    sf::Text instruction(font, "Unscramble the letters and guess the correct word", 22);
    instruction.setPosition({265.f, 250.f});
    instruction.setFillColor(sf::Color(220, 220, 220));

    // ---------------- WORD CARD (SMALLER) ----------------
    sf::RectangleShape wordCard({360.f, 125.f});
    wordCard.setPosition({370.f, 290.f});
    wordCard.setFillColor(wordCardColor);
    wordCard.setOutlineThickness(3.f);
    wordCard.setOutlineColor(accentColor);

    sf::Text wordLabel(font, "scrambled word", 20);
    wordLabel.setPosition({470.f, 307.f});
    wordLabel.setFillColor(sf::Color::White);

    sf::Text scrambledText(font, "", 44);
    scrambledText.setFillColor(accentColor);

    // ---------------- INPUT AREA ----------------
    sf::Text answerLabel(font, "Your Answer:", 24);
    answerLabel.setPosition({220.f, 460.f});
    answerLabel.setFillColor(sf::Color::White);

    sf::RectangleShape inputBox({450.f, 58.f});
    inputBox.setPosition({380.f, 450.f});
    inputBox.setFillColor(sf::Color(245, 245, 245));
    inputBox.setOutlineThickness(3.f);
    inputBox.setOutlineColor(sf::Color::White);

    sf::Text inputText(font, "", 28);
    inputText.setPosition({395.f, 460.f});
    inputText.setFillColor(sf::Color::Black);

    sf::Text placeholderText(font, "type your answer here", 24);
    placeholderText.setPosition({400.f, 462.f});
    placeholderText.setFillColor(sf::Color(130, 130, 130));

    // ---------------- BUTTONS ----------------
    sf::RectangleShape submitButton({150.f, 55.f});
    submitButton.setPosition({260.f, 550.f});
    submitButton.setFillColor(submitColor);

    sf::Text submitText(font, "Submit", 28);
    submitText.setPosition({300.f, 560.f});
    submitText.setFillColor(sf::Color::White);

    sf::RectangleShape nextWordButton({170.f, 55.f});
    nextWordButton.setPosition({470.f, 550.f});
    nextWordButton.setFillColor(nextWordColor);

    sf::Text nextWordText(font, "Next Word", 28);
    nextWordText.setPosition({495.f, 560.f});
    nextWordText.setFillColor(sf::Color::White);

    sf::RectangleShape nextLevelButton({230.f, 60.f});
    nextLevelButton.setPosition({435.f, 545.f});
    nextLevelButton.setFillColor(nextLevelColor);

    sf::Text nextLevelText(font, "Enter Next Level", 28);
    nextLevelText.setPosition({460.f, 558.f});
    nextLevelText.setFillColor(sf::Color::White);

    sf::RectangleShape reattemptButton({230.f, 60.f});
    reattemptButton.setPosition({435.f, 545.f});
    reattemptButton.setFillColor(reattemptColor);

    sf::Text reattemptText(font, "Reattempt Level", 28);
    reattemptText.setPosition({460.f, 558.f});
    reattemptText.setFillColor(sf::Color::White);

    // ---------------- MESSAGE ----------------
    sf::Text message(font, "", 30);
    message.setPosition({180.f, 655.f});

    // ---------------- GAME LOOP ----------------
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // ---------------- MOUSE CLICK ----------------
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mousePos(
                    (float)mouseEvent->position.x,
                    (float)mouseEvent->position.y
                );

                if (!levelCompleted && !gameCompleted && !answerCorrect && !levelFailed) {
                    if (inputBox.getGlobalBounds().contains(mousePos)) {
                        isTyping = true;
                        inputBox.setOutlineColor(sf::Color::Yellow);
                    } else {
                        isTyping = false;
                    }
                }

                if (!levelCompleted && !gameCompleted && !answerCorrect && !levelFailed) {
                    if (submitButton.getGlobalBounds().contains(mousePos)) {
                        if (userInput == currentWord) {
                            answerCorrect = true;
                            score += 10;
                            message.setString("Correct Answer! +10  |  Click Next Word");
                            message.setFillColor(sf::Color::Green);
                            inputBox.setOutlineColor(sf::Color::Green);
                        } else {
                            message.setString("Wrong Answer! Try Again");
                            message.setFillColor(errorColor);
                            inputBox.setOutlineColor(errorColor);
                        }
                    }
                }

                if (!levelCompleted && !gameCompleted && answerCorrect && !levelFailed) {
                    if (nextWordButton.getGlobalBounds().contains(mousePos)) {
                        currentWordIndex++;
                        solvedWordsInLevel++;

                        if (currentWordIndex >= (int)words.size()) {
                            gameCompleted = true;
                            message.setString("Congratulations! You completed the full game");
                            message.setFillColor(sf::Color::Green);
                        } else if (checkLevelComplete()) {
                            levelCompleted = true;
                            message.setString("Congratulations! Level " + to_string(currentLevel) + " Completed. Click Enter Next Level");
                            message.setFillColor(sf::Color::Green);
                        } else {
                            loadCurrentWord(rng);
                            inputBox.setOutlineColor(sf::Color::White);
                            message.setString("");
                            wordClock.restart();
                        }
                    }
                }

                if (levelCompleted && !gameCompleted) {
                    if (nextLevelButton.getGlobalBounds().contains(mousePos)) {
                        currentLevel++;
                        solvedWordsInLevel = 0;
                        levelCompleted = false;
                        levelFailed = false;

                        loadCurrentWord(rng);
                        inputBox.setOutlineColor(sf::Color::White);
                        message.setString("");
                        wordClock.restart();
                    }
                }

                if (levelFailed && !gameCompleted) {
                    if (reattemptButton.getGlobalBounds().contains(mousePos)) {
                        currentWordIndex = getLevelStartIndex(currentLevel);
                        solvedWordsInLevel = 0;
                        levelFailed = false;
                        answerCorrect = false;
                        isTyping = false;

                        loadCurrentWord(rng);
                        inputBox.setOutlineColor(sf::Color::White);
                        message.setString("");
                        wordClock.restart();
                    }
                }
            }

            // ---------------- KEYBOARD INPUT ----------------
            if (isTyping && !answerCorrect && !levelCompleted && !gameCompleted && !levelFailed) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    char ch = (char)textEvent->unicode;

                    if (ch == '\b' && !userInput.empty()) {
                        userInput.pop_back();
                    } else if (isalpha((unsigned char)ch)) {
                        if (userInput.length() < 20) {
                            userInput += tolower(ch);
                        }
                    }

                    inputText.setString(userInput);
                }
            }
        }

        // ---------------- TIMER LOGIC ----------------
        float timeLeft = timePerWord - wordClock.getElapsedTime().asSeconds();

        if (timeLeft < 0) {
            timeLeft = 0;
        }

        if (!answerCorrect && !levelCompleted && !gameCompleted && !levelFailed) {
            if (timeLeft <= 0) {
                levelFailed = true;
                isTyping = false;
                inputBox.setOutlineColor(errorColor);
                message.setString("Time Up! Reattempt Level " + to_string(currentLevel));
                message.setFillColor(errorColor);
            }
        }

        // ---------------- HOVER EFFECT ----------------
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos((float)mousePixel.x, (float)mousePixel.y);

        submitButton.setFillColor(submitColor);
        nextWordButton.setFillColor(nextWordColor);
        nextLevelButton.setFillColor(nextLevelColor);
        reattemptButton.setFillColor(reattemptColor);

        if (submitButton.getGlobalBounds().contains(mousePos) && !answerCorrect && !levelCompleted && !gameCompleted && !levelFailed) {
            submitButton.setFillColor(submitHoverColor);
        }

        if (nextWordButton.getGlobalBounds().contains(mousePos) && answerCorrect && !levelCompleted && !gameCompleted && !levelFailed) {
            nextWordButton.setFillColor(nextWordHoverColor);
        }

        if (nextLevelButton.getGlobalBounds().contains(mousePos) && levelCompleted && !gameCompleted) {
            nextLevelButton.setFillColor(nextLevelHoverColor);
        }

        if (reattemptButton.getGlobalBounds().contains(mousePos) && levelFailed && !gameCompleted) {
            reattemptButton.setFillColor(reattemptHoverColor);
        }

        // ---------------- UPDATE TEXT ----------------
        int totalLevels = (words.size() + wordsPerLevel - 1) / wordsPerLevel;

        levelText.setString("Level: " + to_string(currentLevel) + "/" + to_string(totalLevels));
        scoreText.setString("Score: " + to_string(score));
        progressText.setString("Words Cleared: " + to_string(solvedWordsInLevel) + "/5");
        timerText.setString("Time Left: " + to_string((int)timeLeft) + " sec");

        float progressWidth = (320.0f * solvedWordsInLevel) / 5.0f;
        progressBarFill.setSize({progressWidth, 18.f});

        scrambledText.setString(scrambledWord);

        sf::FloatRect wordBounds = scrambledText.getLocalBounds();
        scrambledText.setPosition({
            550.f - wordBounds.size.x / 2.f,
            345.f - wordBounds.position.y
        });

        inputText.setString(userInput);

        // ---------------- DRAW ----------------
        window.clear(bgColor);

        window.draw(topPanel);
        window.draw(title);
        window.draw(levelText);
        window.draw(scoreText);
        window.draw(progressText);
        window.draw(progressBarBack);
        window.draw(progressBarFill);

        // timer under level
        window.draw(watchCircle);
        window.draw(watchTop);
        window.draw(watchHand1);
        window.draw(watchHand2);
        window.draw(timerText);

        window.draw(instruction);
        window.draw(wordCard);
        window.draw(wordLabel);

        if (!gameCompleted) {
            window.draw(scrambledText);
        }

        if (!levelCompleted && !gameCompleted && !levelFailed) {
            window.draw(answerLabel);
            window.draw(inputBox);

            if (userInput.empty()) {
                window.draw(placeholderText);
            }

            window.draw(inputText);
            window.draw(submitButton);
            window.draw(submitText);

            if (answerCorrect) {
                window.draw(nextWordButton);
                window.draw(nextWordText);
            }
        }

        if (levelCompleted && !gameCompleted) {
            window.draw(nextLevelButton);
            window.draw(nextLevelText);
        }

        if (levelFailed && !gameCompleted) {
            window.draw(reattemptButton);
            window.draw(reattemptText);
        }

        if (gameCompleted) {
            sf::Text finalText(font, "GAME COMPLETED!", 42);
            finalText.setPosition({355.f, 470.f});
            finalText.setFillColor(sf::Color::Green);
            window.draw(finalText);

            sf::Text finalScore(font, "Final Score: " + to_string(score), 32);
            finalScore.setPosition({420.f, 530.f});
            finalScore.setFillColor(sf::Color::White);
            window.draw(finalScore);
        }

        window.draw(message);
        window.display();
    }

    return 0;
}