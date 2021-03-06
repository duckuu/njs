#ifndef __not_jelly_splash__board__
#define __not_jelly_splash__board__

#include <vector>
#include "globalConstants.h"
#include <SFML/Graphics.hpp>
#include "entity.h"
#include "resourceHolder.h"
#include "engine.h"
#include "entityInfo.h"

class cBoard {
public:
    cBoard(cEngine& engine, int x = 7, int y = 9, int bottom = 8, int top = 0);
    
    short               at(int x, int y) const;
    bool                canBlowUp(sf::Vector2i) const;
    void                clear();
    void                clearNode(int x, int y);
    bool                clickable(int x, int y) const;
    bool                closed(int x, int y) const;
    short               colourAt(sf::Vector2i) const;
    bool                diamond(int x, int y) const;
    bool                empty(int x, int y) const;
    void                executeFall(int, int, int, int);
    bool                freeColumn(int x) const;
    bool                fallible(int x, int y) const;
    int                 getHighestDiamond() const;
    int                 getHighestStuck() const;
    bool                guard(int x, int y) const;
    void                init();
    void                makeTriplet(int x, int y);
    void                mark(sf::Vector2i, bool);
    bool                marked(sf::Vector2i) const;
    void                moveEveryone(sf::Vector2f);
    short               neighbourCount(int x, int y) const;
    bool                normal(int x, int y) const;
    cEntity*            piece(int x, int y) const;
    void                place(int x, int y, EntType t, EntColour c = EntColour::random, bool super = false);
    void                reCreate(int x, int y, int bottom, int top);
    void                remove(int x, int y);
    void                resetMarked();
    void                set(int x, int y, short value);
    bool                slime(int x, int y) const;
    bool                valid(int x, int y) const;

private:
    template<class T>
    std::unique_ptr<T>    spawn(const std::string& id)
    {
        std::unique_ptr<T> ptr { new T(rEngine, id) };
        return std::move(ptr);
    }
    void                keepTheBooks(int, int);
    
public:
    int                 mSizeX, mSizeY, mTop, mBottom;

private:
    // mCell stores everything about the piece in one, i.e. its type and colour.
    // -1: not accessible, 0: empty.
    // If not 0:
    // 1 - 5: colour: red, green, blue, yellow, purple.
    // 6: diamond, 7: block, 8: guard
    // So this covers the first 1 bits.
    // The rest are sign bits:
    // bit16: superjelly?
    // bit32: stuckjelly?
    // bit64: slime?
    // To get at these: XOR with 255-16, 255-32, 255-64, jne.

    std::vector<std::vector<short>>                     mCell;
    std::vector<std::vector<std::unique_ptr<cEntity>>>  mPieces;
    std::vector<std::vector<bool>>                      mMarked;
    cEngine&                                            rEngine;
    
};

#endif /* defined(__not_jelly_splash__board__) */
