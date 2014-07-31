#include "entity.h"
#include "globalConstants.h"
#include <cmath>

void cEntity::loadInfo(const std::string& id)
{
    const cEntityInfo& info = rEngine.mEntityHolder.get(id);
    mType = info.mType;
    mColour = info.mColour;
    // Load animations
    
    for ( const auto& i : info.mAnimations )
    {
        cAnimation anim = rEngine.mAnimationHolder.get(i);
        mAnims[anim.mName] = anim;
    }
    
    // Locate texture; set "idle" as basic anim, set anim
    // phase counter to 0
    
    mSprite.setTexture(rEngine.mTextureHolder.get(info.mTexID));
    mSprite.setTextureRect(mAnims["idle"].mFirstPhaseRect);
    pCurrentAnim = &mAnims["idle"];
    mSprite.setScale(gkCellPixSizeX / static_cast<float>(pCurrentAnim->mFirstPhaseRect.width),
                     gkCellPixSizeY / static_cast<float>(pCurrentAnim->mFirstPhaseRect.height));
    mAnimStepCounter = 0;
    mCurrentAnimSteps = pCurrentAnim->mSteps;
}

void cEntity::switchToAnim(const std::string& name)
{
    if ( mAnims.find(name) == mAnims.end() ) return;
    
    pCurrentAnim = &mAnims[name];
    mTimeAccumulated = 0.f;
    mAnimStepCounter = 0;
    mCurrentAnimSteps = pCurrentAnim->mSteps;
}

void cEntity::explode()
{
    switchToAnim("explode");
    mState = EntState::exploding;
}

void cEntity::setGoal(float x, float y)
{    
    mGoal.x = x;    mGoal.y = y;
    sf::Vector2f    d = mGoal - mPos;
    float           dist = sqrt(pow(d.x, 2) + pow(d.y, 2));
    
    d.x /= dist;
    d.y /= dist;
    
    mVel.x = d.x * mSpeed;
    mVel.y = d.y * mSpeed;
    
    mNeedToMove = true;
    mState = EntState::moving;
}

void cEntity::update(float dt)
{
    mTimeAccumulated += dt;
    
    // Update position - if needed
    
    if ( mNeedToMove )
    {
        sf::Vector2f d = mGoal - mPos;
        float dist = (pow(d.x, 2) + pow(d.y, 2));   // first, dist squared, but
        if ( dist < gkNearEnough )                           // you get the idea
        {
            mPos = mGoal;
            mNeedToMove = false;
            mState = EntState::normal;
            mVel.x = 0; mVel.y = 0;
        }
        else
        {
            mPos += mVel * dt;
        }
    }
    
    // Update texture & animate via moving the texture rectangle
    // to wherever it needs to go. If we're exploding,
    // and we reached the last explosion frame, announce that
    // by switching state to dead.
    
    // For now, for diagnostic reasons, change the original:
    // if ( mCurrentAnimSteps > 1 )
    // to:
    
    if ( mCurrentAnimSteps )
    {
        if ( mTimeAccumulated >= pCurrentAnim->mStepTimeInMs )
        {
            mTimeAccumulated = 0.f;
            mAnimStepCounter += 1;
            
            if ( mState == EntState::exploding && mAnimStepCounter >= mCurrentAnimSteps )
            {
                mState = EntState::dead;
                mAnimStepCounter -= 1;      // hold on to last explosion frame
            }
            
            mAnimStepCounter %= mCurrentAnimSteps;
            
            sf::IntRect itmp = pCurrentAnim->mFirstPhaseRect;
            itmp.left += (itmp.width * mAnimStepCounter);
            mSprite.setTextureRect(itmp);
        }
    }
}

void cJelly::switchColour(EntColour c)
{
    mColour = c;
    if ( c == EntColour::random ) { c = static_cast<EntColour>(rand() % 5 + 1); }
    std::string id;
    switch ( c ) {
        case EntColour::red:        id = "redJelly"; break;
        case EntColour::green:      id = "greenJelly"; break;
        case EntColour::yellow:     id = "yellowJelly"; break;
        case EntColour::blue:       id = "blueJelly"; break;
        case EntColour::purple:     id = "purpleJelly"; break;
        default: break;
    }
    loadInfo(id);
}

cJelly::cJelly(cEngine& engine, const std::string& id):
cEntity { engine, id }
{
    mSpeed = gkJellySpeed;
}

void cJelly::makeSuper(Direction dir)
{
    mSuper = true;
    if ( dir == Direction::undecided )
    {
        dir = rand() % 2 == 0 ? Direction::horizontal : Direction::vertical;
    }
    
    mDir = dir;

    pSuperAnim = &mAnims["super"];
    mSuperTimeAccumulated = 0.f;
    mSuperAnimStepCounter = 0;
    mSuperAnimSteps = pSuperAnim->mSteps;

    mSuperSprite.setTexture(rEngine.mTextureHolder.get("assets"));
    mSuperSprite.setTextureRect(pSuperAnim->mFirstPhaseRect);
    mSuperSprite.setScale( gkCellPixSizeX / pSuperAnim->mFirstPhaseRect.width,
                           gkCellPixSizeY / pSuperAnim->mFirstPhaseRect.height );

    
}

void cJelly::render(sf::RenderWindow& window)
{
    if ( mSuper )
    {
        mSuperSprite.setPosition(mPos);
        window.draw(mSuperSprite);
    }
    mSprite.setPosition(mPos);
    window.draw(mSprite);
}
