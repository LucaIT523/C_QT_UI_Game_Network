#ifndef DRAWABLEBUTTON_H
#define DRAWABLEBUTTON_H

#include <QPushButton>
#include <QPixmap>
#include <QPainter>



class DrawableButton : public QPushButton {
public:
    DrawableButton(QPixmap normalImage, QPixmap hoverImage, QPixmap pressedImage, QPixmap disabledImage, QWidget *parent = nullptr, bool isEnable=true) : QPushButton(parent) {
        setFixedSize(100, 100); // Set the button size
        this->normalImage = normalImage;
        this->hoverImage = hoverImage;
        this->pressedImage = pressedImage;
        this->disabledImage = disabledImage;
        this->isEnable = isEnable;
    }
    void setEnable(bool isEnable){
        this->isEnable = isEnable;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw different images based on the button state
        if (underMouse()) {
            painter.drawPixmap(rect(), hoverImage);
        } else if (isDown()) {
            painter.drawPixmap(rect(), pressedImage);
        } else {
            painter.drawPixmap(rect(), normalImage);
        }
    }

private:
    QPixmap normalImage;
    QPixmap hoverImage;
    QPixmap pressedImage;
    QPixmap disabledImage;
    bool isEnable;
};
#endif // DRAWABLEBUTTON_H
