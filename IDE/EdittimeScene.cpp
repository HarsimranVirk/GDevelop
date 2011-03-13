#include "EdittimeScene.h" //Must be placed first

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "GDL/RuntimeScene.h"
#include "GDL/Game.h"
#include "GDL/Chercher.h"
#include <vector>

EdittimeScene::EdittimeScene(sf::RenderWindow * renderWindow_, RuntimeGame * game_) :
runtimeScene(renderWindow_, game_),
editing(true),
view( sf::FloatRect( 0.0f, 0.0f, runtimeScene.game->windowWidth, runtimeScene.game->windowHeight ) ),
grid( false ),
snap( false),
gridWidth( 32 ),
gridHeight( 32 ),
gridR( 158 ),
gridG( 180 ),
gridB( 255 ),
windowMask(false),
isMovingObject( false ),
isResizingX( false ),
isResizingY( false ),
xRectangleSelection(0),
yRectangleSelection(0),
xEndRectangleSelection(0),
yEndRectangleSelection(0),
colorGUI( 0 ),
colorPlus( true ),
isMoving( false ),
isSelecting(false),
deplacementOX( 0 ),
deplacementOY( 0 )
{
    //ctor
    runtimeScene.renderWindow->SetView(view);
    runtimeScene.running = false;
}

EdittimeScene::~EdittimeScene()
{
    //dtor
}


////////////////////////////////////////////////////////////
/// Affichage de la sc�ne en mode �dition, � son d�part.
////////////////////////////////////////////////////////////
void EdittimeScene::RenderEdittimeScene()
{
    runtimeScene.ManageRenderTargetEvents();

    runtimeScene.renderWindow->Clear( sf::Color( runtimeScene.backgroundColorR, runtimeScene.backgroundColorG, runtimeScene.backgroundColorB ) );
    runtimeScene.renderWindow->SetView(view);

    glClear(GL_DEPTH_BUFFER_BIT);
    runtimeScene.renderWindow->SaveGLStates(); //To allow using OpenGL to draw

    UpdateGUI();

    //On trie les objets par leurs plans
    ObjList allObjects = runtimeScene.objectsInstances.GetAllObjects();
    runtimeScene.OrderObjectsByZOrder( allObjects );

    std::vector < sf::Shape > GUIelements;

    for (unsigned int layerIndex =0;layerIndex<runtimeScene.layers.size();++layerIndex)
    {
        if ( runtimeScene.layers.at(layerIndex).GetVisibility() )
        {
            //Prepare OpenGL rendering
            runtimeScene.renderWindow->RestoreGLStates();

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(runtimeScene.oglFOV, static_cast<double>(runtimeScene.renderWindow->GetWidth())/static_cast<double>(runtimeScene.renderWindow->GetHeight()), runtimeScene.oglZNear, runtimeScene.oglZFar);

            glViewport(0,0, runtimeScene.renderWindow->GetWidth(), runtimeScene.renderWindow->GetHeight());

            runtimeScene.renderWindow->SaveGLStates();

            //Render all objects
            for (unsigned int id = 0;id < allObjects.size();++id)
            {
                if ( allObjects[id]->GetLayer() == runtimeScene.layers.at(layerIndex).GetName())
                {
                    allObjects[id]->DrawEdittime(*runtimeScene.renderWindow);

                    //Selection rectangle
                    if ( find(objectsSelected.begin(), objectsSelected.end(), allObjects[id]) != objectsSelected.end() )
                    {
                        sf::Shape selection = sf::Shape::Rectangle( 0, 0,
                                                                   allObjects[id]->GetWidth(),
                                                                   allObjects[id]->GetHeight(),
                                                                   sf::Color( 0, 0, 200, 40 ), 1, sf::Color( 0, 0, 255, 128 ) );

                        selection.SetPosition( allObjects[id]->GetDrawableX(),
                                              allObjects[id]->GetDrawableY() );
                        GUIelements.push_back( selection );

                        if ( objectsSelected.size() == 1)
                        {
                            sf::Shape resizeXBt = sf::Shape::Rectangle( 0, 0, 4, 4, sf::Color( 255, 255, 255, 255 ), 1, sf::Color( 0, 0, 255, 255 ) );
                            sf::Shape resizeYBt = sf::Shape::Rectangle( 0, 0, 4, 4, sf::Color( 255, 255, 255, 255 ), 1, sf::Color( 0, 0, 255, 255 ) );
                            sf::Shape angleBt = sf::Shape::Rectangle( 0, 0, 4, 4, sf::Color( 255, 255, 255, 255 ), 1, sf::Color( 0, 0, 255, 255 ) );
                            sf::Shape center = sf::Shape::Circle( 0, 0, 2, sf::Color( 0, 0, 255, 255 ), 1, sf::Color( 0, 0, 255, 255 ) );
                            sf::Shape centerToAngle = sf::Shape::Line(0,0, 20, 0, 1, sf::Color( 0, 0, 255, 255 ), 0, sf::Color( 0, 0, 255, 255 ) );

                            resizeXBt.SetPosition( allObjects[id]->GetDrawableX()+allObjects[id]->GetWidth()-4,
                                                  allObjects[id]->GetDrawableY()+allObjects[id]->GetHeight()/2-2 );

                            resizeYBt.SetPosition( allObjects[id]->GetDrawableX()+allObjects[id]->GetWidth()/2-2,
                                                  allObjects[id]->GetDrawableY()+allObjects[id]->GetHeight()-4 );

                            center.SetPosition(allObjects[id]->GetDrawableX()+allObjects[id]->GetWidth()/2,
                                               allObjects[id]->GetDrawableY()+allObjects[id]->GetHeight()/2);

                            angleBt.SetPosition(    allObjects[id]->GetDrawableX()+allObjects[id]->GetWidth()/2-2
                                                    +20*cos(allObjects[id]->GetAngle()/180.f*3.14159),
                                                    allObjects[id]->GetDrawableY()+allObjects[id]->GetHeight()/2-2
                                                    +20*sin(allObjects[id]->GetAngle()/180.f*3.14159) );

                            centerToAngle.Rotate(-allObjects[id]->GetAngle());
                            centerToAngle.SetPosition(  allObjects[id]->GetDrawableX()+allObjects[id]->GetWidth()/2,
                                                        allObjects[id]->GetDrawableY()+allObjects[id]->GetHeight()/2);

                            GUIelements.push_back( centerToAngle );
                            GUIelements.push_back( center );
                            GUIelements.push_back( angleBt );
                            GUIelements.push_back( resizeXBt );
                            GUIelements.push_back( resizeYBt );

                        }
                    }
                }
            }
        }
    }

    //Affichage de la grille
    if ( grid )
        RenderGrid();

    //Draw GUI Elements
    for (unsigned int i = 0;i<GUIelements.size();++i)
    	runtimeScene.renderWindow->Draw(GUIelements[i]);

    if ( isSelecting )
    {
        sf::Shape selection = sf::Shape::Rectangle(xRectangleSelection, yRectangleSelection,
                                                   xEndRectangleSelection-xRectangleSelection, yEndRectangleSelection-yRectangleSelection,
                                                   sf::Color( 0, 0, 200, 40 ), 1, sf::Color( 0, 0, 255, 128 ) );
        runtimeScene.renderWindow->Draw(selection);
    }

    //Affichage de l'objet � ins�rer en semi transparent
    if ( !objectToAdd.empty() )
    {
        try
        {
            ObjSPtr object = boost::shared_ptr<Object>();

            if ( Picker::PickOneObject( &runtimeScene.initialObjects, objectToAdd ) != -1)
                object = runtimeScene.initialObjects[Picker::PickOneObject( &runtimeScene.initialObjects, objectToAdd ) ];
            else if ( Picker::PickOneObject( &runtimeScene.game->globalObjects, objectToAdd ) != -1)
                object = runtimeScene.game->globalObjects[Picker::PickOneObject( &runtimeScene.game->globalObjects, objectToAdd ) ];

            if ( object != boost::shared_ptr<Object>() )
            {
                //Changing an initial object position is not dangerous,
                //as objects created from initial objects are always placed
                //to some coordinates just after their creations.
                if ( grid && snap )
                {
                    object->SetX( static_cast<int>(runtimeScene.renderWindow->ConvertCoords(runtimeScene.input->GetMouseX(), 0).x/gridWidth)*gridWidth );
                    object->SetY( static_cast<int>(runtimeScene.renderWindow->ConvertCoords(0, runtimeScene.input->GetMouseY()).y/gridHeight)*gridHeight );
                }
                else
                {
                    object->SetX( runtimeScene.renderWindow->ConvertCoords(runtimeScene.input->GetMouseX(), 0).x );
                    object->SetY( runtimeScene.renderWindow->ConvertCoords(0, runtimeScene.input->GetMouseY()).y );
                }

                object->DrawEdittime( *runtimeScene.renderWindow );
            }
        }
        catch ( ... ) { }
    }


    if ( windowMask )
    {
        sf::Shape windowMaskShape = sf::Shape::Rectangle(view.GetCenter().x-runtimeScene.game->windowWidth/2, view.GetCenter().y-runtimeScene.game->windowHeight/2,
                                                         runtimeScene.game->windowWidth, runtimeScene.game->windowHeight, sf::Color( 0, 0, 0, 0 ), 1, sf::Color( 255, 255, 255, 128 ) );

        runtimeScene.renderWindow->Draw(windowMaskShape);
        runtimeScene.renderWindow->SetView(view);
    }

    runtimeScene.renderWindow->RestoreGLStates();
    runtimeScene.renderWindow->Display();
}

////////////////////////////////////////////////////////////
/// Affichage d'une grille
////////////////////////////////////////////////////////////
void EdittimeScene::RenderGrid()
{
    int departX = static_cast<int>((view.GetCenter().x-view.GetSize().x/2) / gridWidth)-gridWidth;
    departX *= gridWidth;
    int positionX = departX;
    int departY = static_cast<int>((view.GetCenter().y-view.GetSize().y/2) / gridHeight)-gridHeight;
    departY *= gridHeight;
    int positionY = departY;

    for ( positionX = departX;positionX < (view.GetCenter().x+view.GetSize().x/2) ; positionX += gridWidth )
    {
        sf::Shape line = sf::Shape::Line( positionX, departY, positionX, (view.GetCenter().y+view.GetSize().y/2), 1, sf::Color( gridR, gridG, gridB ));

        runtimeScene.renderWindow->Draw( line );
    }

    for ( positionY = departY;positionY < (view.GetCenter().y+view.GetSize().y/2) ; positionY += gridHeight )
    {
        sf::Shape line = sf::Shape::Line( departX, positionY, (view.GetCenter().x+view.GetSize().x/2), positionY, 1, sf::Color( gridR, gridG, gridB ));

        runtimeScene.renderWindow->Draw( line );
    }
}

////////////////////////////////////////////////////////////
/// Met � jour les couleurs de l'interface
////////////////////////////////////////////////////////////
void EdittimeScene::UpdateGUI()
{
    float elapsedTime = runtimeScene.renderWindow->GetFrameTime();
    if ( colorPlus )
    {
        colorGUI += static_cast<int>(150 * elapsedTime);

        if ( colorGUI > 255 )
        {
            colorGUI = 255;
            colorPlus = false;
        }
    }
    else
    {
        colorGUI -= static_cast<int>(150 * elapsedTime);

        if ( colorGUI < 10 )
        {
            colorGUI = 10;
            colorPlus = true;
        }
    }
    return;
}

////////////////////////////////////////////////////////////
/// Cherche et renvoie l'ID du plus petit objet sous le curseur
////////////////////////////////////////////////////////////
ObjSPtr EdittimeScene::FindSmallestObject()
{
    ObjList potentialObjects;
    int x = runtimeScene.renderWindow->ConvertCoords(runtimeScene.input->GetMouseX(), 0).x;
    int y = runtimeScene.renderWindow->ConvertCoords(0, runtimeScene.input->GetMouseY()).y;

    ObjList allObjects = runtimeScene.objectsInstances.GetAllObjects();

    for (unsigned int id = 0;id < allObjects.size();++id)
    {
        if ( allObjects[id]->GetDrawableX() < x &&
                allObjects[id]->GetDrawableY() < y &&
                allObjects[id]->GetDrawableX() + allObjects[id]->GetWidth() > x &&
                allObjects[id]->GetDrawableY() + allObjects[id]->GetHeight() > y &&
                runtimeScene.GetLayer(allObjects[id]->GetLayer()).GetVisibility())
        {
            potentialObjects.push_back( allObjects[id] );
        }
    }
    if ( potentialObjects.empty() ) return boost::shared_ptr<Object> (); //Aucun objet trouv�

    ObjSPtr smallest = potentialObjects[0]; //1er objet par d�faut
    if ( potentialObjects.size() > 1 )
    {
        int compare = 1;
        for ( unsigned int j = 0;j < potentialObjects.size();j++ )
        {
            if (( potentialObjects[j]->GetWidth() * potentialObjects[j]->GetHeight() ) <
                    ( potentialObjects[compare]->GetWidth() * potentialObjects[compare]->GetHeight() ) )
            {
                smallest = potentialObjects.at( j );

                compare = j;
            }
            else
            {
                smallest = potentialObjects.at( compare );
            }
        }
    }

    return smallest;
}
