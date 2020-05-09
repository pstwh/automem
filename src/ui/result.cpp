#include "result.h"

char value[8];

Result::Result(LiquidCrystal_I2C *lcd, E *e, char *script)
{
    this->lcd = lcd;
    this->script = script;
    this->cursor = new int(0);
    this->type = ResultType::Show;
}

Result::~Result() {}

void Result::setup()
{
    Parser::run(value, "(m (l (r 24)) (f (r 25)) (l (r 26)) (f (r 27)))");
}

void Result::render()
{
    lcd->createChar(0, Symbols::square);
    lcd->createChar(1, Symbols::enter);
    lcd->createChar(2, Symbols::arrow_up);

    lcd->setCursor(1, 1);
    lcd->print("Codigo:");
    lcd->setCursor(1, 2);
    lcd->print(value);
    lcd->setCursor(19, 3);
    lcd->write(1);

    if (type == ResultType::Edit)
    {
        lcd->setCursor(*cursor + 1, 3);
        lcd->write(2);
    }
}

void Result::action(LinkedList<View *> *views)
{
    Controls c = Controls::getInstance();

    if (type == ResultType::Edit)
    {
        if (c.up->update() && c.up->rose())
        {
            value[*cursor]++;
        }

        if (c.down->update() && c.down->rose())
        {
            value[*cursor]--;
        }

        if (c.right->update() && c.right->rose())
        {
            lcd->setCursor(*cursor + 1, 3);
            lcd->print(" ");
            *cursor += 1;
            *cursor %= 4;
        }

        if (c.left->update() && c.left->rose())
        {
            lcd->setCursor(*cursor + 1, 3);
            lcd->print(" ");
            *cursor -= 1;
            if (*cursor < 0)
            {
                *cursor = 0;
            }
        }
    }

    if (c.enter->update() && c.enter->rose())
    {
        if (type == ResultType::Show)
        {
            type = ResultType::Edit;
        }
        else if (type == ResultType::Edit)
        {
            lcd->clear();
            type = ResultType::Show;
            Serial.println(value);
        }
    }

    if (c.back->update() && c.back->rose())
    {
        if (type == ResultType::Show)
        {
            lcd->clear();
            views->pop();
        }
        else if (ResultType::Edit)
        {
            lcd->clear();
            type = ResultType::Show;
        }
    }
}
