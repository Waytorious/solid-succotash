#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

//удобненько
enum class AppState { Intro, Editor };

// функция для генерации полного спектра (HSV -> RGB)
sf::Color hsvToRgb(float h, float s, float v) {
    int i = static_cast<int>(h / 60.0f) % 6;
    float f = (h / 60.0f) - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    v *= 255; p *= 255; q *= 255; t *= 255;
    
    switch (i) {
        case 0: return sf::Color(v, t, p);
        case 1: return sf::Color(q, v, p);
        case 2: return sf::Color(p, v, t);
        case 3: return sf::Color(p, q, v);
        case 4: return sf::Color(t, p, v);
        case 5: return sf::Color(v, p, q);
    }
    return sf::Color::Black;
}


// КЛАССЫ Я ЛЮБЛЮ КЛАССЫ Я ОБОЖАЮ КЛАССЫ

//базовый класс, от него все пойдет
class Figure {
protected:
    //цвет, заполнять?(y/n), толщина границы, размер
    sf::Color color;
    bool isFilled;
    float thickness;
    float size;
public:
    Figure(sf::Color c, bool filled, float thick, float s) 
        : color(c), isFilled(filled), thickness(thick), size(s) {}
    virtual ~Figure() = default;
    
    //чтобы абстрактный был
    virtual void draw(sf::RenderWindow& window) = 0;
};

//кружочек (первый примитив)
class CircleFigure : public Figure {
private:
    sf::CircleShape shape;
public:
    CircleFigure(sf::Vector2f pos, sf::Color c, bool filled, float thick, float s) 
        : Figure(c, filled, thick, s) {
        shape.setRadius(size);
        shape.setOrigin({size, size}); 
        shape.setPosition(pos);
        shape.setFillColor(isFilled ? color : sf::Color::Transparent);
        shape.setOutlineColor(color);
        shape.setOutlineThickness(thickness);
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

//прямоугольничек (второй примитив)
class RectFigure : public Figure {
private:
    sf::RectangleShape shape;
public:
    RectFigure(sf::Vector2f pos, sf::Color c, bool filled, float thick, float s) 
        : Figure(c, filled, thick, s) {
        shape.setSize({size * 2.f, size * 1.2f});
        shape.setOrigin({size, size * 0.6f}); 
        shape.setPosition(pos);
        shape.setFillColor(isFilled ? color : sf::Color::Transparent);
        shape.setOutlineColor(color);
        shape.setOutlineThickness(thickness);
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

//треугольничек (третий примитив)
class TriangleFigure : public Figure {
private:
    //хоба
    sf::CircleShape shape;
public:
    TriangleFigure(sf::Vector2f pos, sf::Color c, bool filled, float thick, float s) 
        : Figure(c, filled, thick, s) {
        shape.setRadius(size * 1.1f);
        shape.setPointCount(3);
        shape.setOrigin({size * 1.1f, size * 1.1f}); 
        shape.setPosition(pos);
        shape.setFillColor(isFilled ? color : sf::Color::Transparent);
        shape.setOutlineColor(color);
        shape.setOutlineThickness(thickness);
    }
    void draw(sf::RenderWindow& window) override { window.draw(shape); }
};

// MAIN MAIN MAIN MAIN MAIN
int main() {
    sf::RenderWindow window(sf::VideoMode({1000, 750}), "Vector Editor - Have Fun!!!!!!!");
    window.setFramerateLimit(60);

    AppState state = AppState::Intro;

    sf::Font font;
    // я ненавижу шрифты (путь мой личный тут, поменять потом)
    if (!font.openFromFile(R"(/Users/waytorious/Desktop/военмех/1 курс/2 семестр/информационные технологии и программирование/пр5/arial.ttf)")) {
        std::cout << "Failed to load font!" << std::endl;
        return -1;
    }

    //текст заставки
    sf::Text introText(font, 
        "PRACTICAL WORK 5 (Advanced Level)\n"
        "Developer: Variant 9\n\n"
        "Goal: Vector Graphics Editor Implementation\n\n"
        "Controls:\n"
        "  [LMB] - Select tools, pick colors, drag sliders\n"
        "  [LMB] - Draw shape on canvas\n\n"
        "Press [SPACE] to enter the editor.", 22);
    introText.setFillColor(sf::Color::White);
    introText.setPosition({100.f, 100.f});

    // 500 фигур можно будет короче а больше нельзя атата
    Figure* shapes[500];
    int shapeCount = 0;

    // параметры кисти
    int currentTool = 1; 
    //по умолчанию красненький
    sf::Color currentBrushColor = sf::Color::Red;
    bool isFilled = true;
    float currentSize = 40.f;       // 10 - 100
    float currentThickness = 2.f;   // 0 - 20

    // флаги состояния ползунков (перетаскивание)
    bool isDraggingSize = false;
    bool isDraggingThick = false;

    // ахаха трехстрочный комментарий
    // тут расписан ИНТЕРФЕЙС
    // ахаха трехстрочный комментарий

    float menuHeight = 80.f;
    sf::RectangleShape menuBar({(float)window.getSize().x, menuHeight});
    menuBar.setFillColor(sf::Color(45, 45, 45));

    // кнопки выбора формы
    sf::RectangleShape btnCircle({40.f, 40.f}); btnCircle.setPosition({20.f, 20.f}); btnCircle.setFillColor(sf::Color(80, 80, 80));
    sf::RectangleShape btnRect({40.f, 40.f});   btnRect.setPosition({70.f, 20.f}); btnRect.setFillColor(sf::Color(80, 80, 80));
    sf::RectangleShape btnTri({40.f, 40.f});    btnTri.setPosition({120.f, 20.f}); btnTri.setFillColor(sf::Color(80, 80, 80));

    // иконки выбора формы (поверх кнопок)
    sf::CircleShape iconCircle(12.f); iconCircle.setOrigin({12.f, 12.f}); iconCircle.setPosition({40.f, 40.f}); iconCircle.setFillColor(sf::Color::White);
    sf::RectangleShape iconRect({24.f, 16.f}); iconRect.setOrigin({12.f, 8.f}); iconRect.setPosition({90.f, 40.f}); iconRect.setFillColor(sf::Color::White);
    sf::CircleShape iconTri(14.f, 3); iconTri.setOrigin({14.f, 14.f}); iconTri.setPosition({140.f, 43.f}); iconTri.setFillColor(sf::Color::White);

    // цветовой спектр
    sf::Image spectrumImg;
    spectrumImg.resize({200, 40});
    for (unsigned int x = 0; x < 200; ++x) {
        float hue = (x / 200.f) * 360.f;
        sf::Color col = hsvToRgb(hue, 1.0f, 1.0f);
        for (unsigned int y = 0; y < 40; ++y) { spectrumImg.setPixel({x, y}, col); }
    }
    sf::Texture spectrumTex; spectrumTex.loadFromImage(spectrumImg);
    sf::Sprite spectrumSprite(spectrumTex); spectrumSprite.setPosition({180.f, 20.f});
    
    // рамка для спектра (sprite же просто так обвести нельзя правильно зачем)
    sf::RectangleShape spectrumFrame({200.f, 40.f});
    spectrumFrame.setPosition({180.f, 20.f});
    spectrumFrame.setFillColor(sf::Color::Transparent);
    spectrumFrame.setOutlineThickness(2.f); spectrumFrame.setOutlineColor(sf::Color::White);

    // индикатор выбранного цвета
    sf::RectangleShape colorIndicator({40.f, 40.f});
    colorIndicator.setPosition({395.f, 20.f});
    colorIndicator.setOutlineThickness(2.f); colorIndicator.setOutlineColor(sf::Color::White);

    // заливка
    sf::RectangleShape btnFill({60.f, 40.f}); btnFill.setPosition({450.f, 20.f}); btnFill.setFillColor(sf::Color(80, 80, 80));
    sf::Text txtFill(font, "Fill", 16); txtFill.setFillColor(sf::Color::White); txtFill.setPosition({465.f, 30.f});

    // ползуночек размера
    sf::RectangleShape sizeTrack({100.f, 6.f}); sizeTrack.setPosition({530.f, 45.f}); sizeTrack.setFillColor(sf::Color(100, 100, 100));
    sf::Text txtSize(font, "Size:", 14); txtSize.setFillColor(sf::Color::White); txtSize.setPosition({530.f, 20.f});
    sf::CircleShape sizeThumb(8.f); sizeThumb.setOrigin({8.f, 8.f}); sizeThumb.setFillColor(sf::Color::White);
    // ползуночек толщины границы
    sf::RectangleShape thickTrack({100.f, 6.f}); thickTrack.setPosition({660.f, 45.f}); thickTrack.setFillColor(sf::Color(100, 100, 100));
    sf::Text txtThick(font, "Thick:", 14); txtThick.setFillColor(sf::Color::White); txtThick.setPosition({660.f, 20.f});
    sf::CircleShape thickThumb(8.f); thickThumb.setOrigin({8.f, 8.f}); thickThumb.setFillColor(sf::Color::White);

    // хитбоксы для ползунков
    sf::FloatRect sizeHitbox({530.f, 20.f}, {100.f, 40.f});
    sf::FloatRect thickHitbox({660.f, 20.f}, {100.f, 40.f});

    // большая зеленая такая кнопка SAVE
    sf::RectangleShape btnSave({70.f, 40.f}); btnSave.setPosition({790.f, 20.f}); btnSave.setFillColor(sf::Color(0, 150, 0));
    sf::Text txtSave(font, "SAVE", 16); txtSave.setFillColor(sf::Color::White); txtSave.setPosition({802.f, 30.f});

    // уведомление об успешном сохранении
    bool showSaveMessage = false;
    sf::Clock saveMessageClock;
    sf::Text saveSuccessText(font, "Successfully saved to result.png!", 20);
    saveSuccessText.setFillColor(sf::Color::White);
    saveSuccessText.setPosition({20.f, 95.f}); 

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) { window.close(); }

            // тут чтобы окно не пережевывало при изменении размера
            if (event->is<sf::Event::Resized>()) {
                auto resized = event->getIf<sf::Event::Resized>();
                sf::FloatRect visibleArea({0.f, 0.f}, {(float)resized->size.x, (float)resized->size.y});
                window.setView(sf::View(visibleArea));
                menuBar.setSize({(float)resized->size.x, menuHeight}); 
            }

            //кидаем из интро в сам редактор
            if (state == AppState::Intro) {
                if (event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space) {
                    state = AppState::Editor;
                }
            }
            else if (state == AppState::Editor) {
                
                // читаем нажатия мыши
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    auto mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        sf::Vector2f mousePos = { (float)mouseEvent->position.x, (float)mouseEvent->position.y };
                        
                        // клик по меню
                        if (mousePos.y <= menuHeight) {
                            
                            // выбор фигуры
                            if (btnCircle.getGlobalBounds().contains(mousePos)) currentTool = 1;
                            if (btnRect.getGlobalBounds().contains(mousePos))   currentTool = 2;
                            if (btnTri.getGlobalBounds().contains(mousePos))    currentTool = 3;

                            // выбор цвета
                            if (spectrumSprite.getGlobalBounds().contains(mousePos)) {
                                // вычисляем локальные координаты клика внутри картинки спектра
                                unsigned int localX = std::clamp(static_cast<unsigned int>(mousePos.x - spectrumSprite.getPosition().x), 0u, 199u);
                                unsigned int localY = std::clamp(static_cast<unsigned int>(mousePos.y - spectrumSprite.getPosition().y), 0u, 39u);
                                // читаем цвет пикселя прям из картинки
                                currentBrushColor = spectrumImg.getPixel({localX, localY});
                            }

                            // переключение заливки
                            if (btnFill.getGlobalBounds().contains(mousePos)) isFilled = !isFilled;
                            
                            // сохранение рисунка
                            if (btnSave.getGlobalBounds().contains(mousePos)) {
                                sf::Texture texture;
                                if (texture.resize(window.getSize())) {
                                    texture.update(window);
                                    if (texture.copyToImage().saveToFile("result.png")) {
                                        showSaveMessage = true; saveMessageClock.restart(); 
                                    }
                                }
                            }

                            // ползунки ползуночки
                            if (sizeHitbox.contains(mousePos)) {
                                isDraggingSize = true;
                                float newX = std::clamp(mousePos.x, 530.f, 630.f); // Ограничиваем пределы
                                currentSize = 10.f + ((newX - 530.f) / 100.f) * 90.f; // Масштабируем до 10-100
                            }
                            if (thickHitbox.contains(mousePos)) {
                                isDraggingThick = true;
                                float newX = std::clamp(mousePos.x, 660.f, 760.f);
                                currentThickness = ((newX - 660.f) / 100.f) * 20.f; // Масштабируем до 0-20
                            }
                        } 
                        // рисуем фигуру если клик был на холсте и ползунки никто не трогает
                        else if (!isDraggingSize && !isDraggingThick) {
                            if (shapeCount < 500) {
                                if (currentTool == 1) shapes[shapeCount] = new CircleFigure(mousePos, currentBrushColor, isFilled, currentThickness, currentSize);
                                else if (currentTool == 2) shapes[shapeCount] = new RectFigure(mousePos, currentBrushColor, isFilled, currentThickness, currentSize);
                                else if (currentTool == 3) shapes[shapeCount] = new TriangleFigure(mousePos, currentBrushColor, isFilled, currentThickness, currentSize);
                                shapeCount++;
                            }
                        }
                    }
                }
                
                // отпускание кнопки мыши (хватит тащить ползуночки)
                else if (event->is<sf::Event::MouseButtonReleased>()) {
                    auto mouseEvent = event->getIf<sf::Event::MouseButtonReleased>();
                    if (mouseEvent->button == sf::Mouse::Button::Left) {
                        isDraggingSize = false;
                        isDraggingThick = false;
                    }
                }

                // движение мыши (перетаскивание ползуночков)
                else if (event->is<sf::Event::MouseMoved>()) {
                    auto mouseEvent = event->getIf<sf::Event::MouseMoved>();
                    sf::Vector2f mousePos = { (float)mouseEvent->position.x, (float)mouseEvent->position.y };

                    if (isDraggingSize) {
                        float newX = std::clamp(mousePos.x, 530.f, 630.f);
                        currentSize = 10.f + ((newX - 530.f) / 100.f) * 90.f; 
                    }
                    if (isDraggingThick) {
                        float newX = std::clamp(mousePos.x, 660.f, 760.f);
                        currentThickness = ((newX - 660.f) / 100.f) * 20.f; 
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        if (state == AppState::Intro) {
            window.draw(introText);
        } 
        else if (state == AppState::Editor) {
            
            // отрисовка холста + фигур
            for (int i = 0; i < shapeCount; ++i) { shapes[i]->draw(window); }

            // отрисовка панели меню
            window.draw(menuBar);

            // обновление после изменений и выборов в меню
            colorIndicator.setFillColor(currentBrushColor);
            btnFill.setOutlineThickness(isFilled ? 2.f : 0.f); btnFill.setOutlineColor(sf::Color::White);
            txtSize.setString("Size: " + std::to_string((int)currentSize));
            txtThick.setString("Thick: " + std::to_string((int)currentThickness));

            // расположение кружочков ползунков
            sizeThumb.setPosition({530.f + ((currentSize - 10.f) / 90.f) * 100.f, 48.f});
            thickThumb.setPosition({660.f + (currentThickness / 20.f) * 100.f, 48.f});

            // подсветка выбранной фигуры
            btnCircle.setOutlineThickness(currentTool == 1 ? 2.f : 0.f); btnCircle.setOutlineColor(sf::Color::White);
            btnRect.setOutlineThickness(currentTool == 2 ? 2.f : 0.f);   btnRect.setOutlineColor(sf::Color::White);
            btnTri.setOutlineThickness(currentTool == 3 ? 2.f : 0.f);    btnTri.setOutlineColor(sf::Color::White);

            // отрисовка кнопок инструментов с иконками
            window.draw(btnCircle); window.draw(iconCircle);
            window.draw(btnRect);   window.draw(iconRect);
            window.draw(btnTri);    window.draw(iconTri);

            window.draw(spectrumSprite);
            window.draw(spectrumFrame);
            window.draw(colorIndicator);

            window.draw(btnFill); window.draw(txtFill);

            // отрисовка ползунков
            window.draw(txtSize); window.draw(sizeTrack); window.draw(sizeThumb);
            window.draw(txtThick); window.draw(thickTrack); window.draw(thickThumb);

            window.draw(btnSave); window.draw(txtSave);

            // отрисовка уведомления о сохранении (исчезает через 1.5 секунды ибо ну а че на него долго смотреть)
            if (showSaveMessage) {
                if (saveMessageClock.getElapsedTime().asSeconds() < 1.5f) {
                    window.draw(saveSuccessText);
                } else { showSaveMessage = false; }
            }
        }

        window.display();
    }

    // перед выходом все за собой чистим
    for (int i = 0; i < shapeCount; ++i) { delete shapes[i]; }
    return 0;
}