// Copyright (C) 2008-2011 by Philipp Muenzel. All rights reserved
// Released under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, Inc.

#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <string>
#include <list>
#include <vector>

#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "dataref.h"
#include "namespaces.h"

namespace PPLNAMESPACE {

/**
  * A convenient way to display a message box with a title and a longer text message
  * that quits X-Plane when closed. This is a very good way to handle improper setups,
  * blame the user, and force quit afterwards
  * @version 1.0
  * @author (c) 2009-2011 by Philipp Muenzel
  */
class MessageWindow
{
public:
    /**
      * This exception is thrown during setup of a Message box, if the requested
      * size of the box is too large to fit in the simulator window.
      */
    class BoxOutOfBoundsException: public std::runtime_error {
    public:
        BoxOutOfBoundsException(const std::string& msg):
            std::runtime_error(msg)
        {}
    };

    /**
      * construct message box centered in the X-Plane window, with title and auto-breaking message
      * @param width in pixels
      * @param height in pixels
      * @param title text in the titlebar
      * @param message text gets automatically br'd according to window width
      * @param quit whether the simulator should be closed after the message was confirmed
      */
    MessageWindow(int width, int height, const std::string& title, const std::string& message, bool quit);

    /**
      * destroy window and all child windows
      */
    ~MessageWindow();

    /**
      * @param width in pixels
      * @exception BoxOutOfBoundsException if width is inappropriate
      */
    void setWidth(int width);

    /**
      * @param height in pixels
      * @exception BoxOutOfBoundsException if height is inappropriate
      */
    void setHeight(int height);

    /**
      * @param title to show in the title bar of the message box
      */
    void setTitle(const std::string& title);

    /**
      * @param message to show in the message box
      */
    void setMessage(const std::string& message);

    /**
      * @param quit quit the sime after the user confirms the dialog? yes or no
      */
    void quitSimOnConfirm(bool quit);

    /**
      * display the window in the sim, forces user to acknownledge the dialog
      */
    void display();

    /**
      * @return is the window currently drawn by X-Plane
      */
    bool isDisplayed();

    /**
      * internal processing of message from X-Plane's widget system
      * it was already figured out that we are adressed
      * @param message
      * @param param1
      * @param param2
      */
    int processMessages(XPWidgetMessage, intptr_t, intptr_t);

    /**
      * static widget callback to register in X-Plane's widget logic
      * retrieves the instance via a pointer-to-object stored in the widget struct
      */
    static int widgetCallback(XPWidgetMessage, XPWidgetID, intptr_t, intptr_t);

private:

    /**
      * split a string in words and store them in the list
      * @param L list of strings where words are stored
      * @param seq original string to split up
      * @param _1cdelim string that declares a splitting point
      * @param _removews remove white spaces
      */
    int splitStr(std::list<std::string>& L, const std::string& seq, const std::string& _1cdelim, bool _removews );

    /**
      * create the box widget with title bar and close buttons
      */
    void createSurroundingBox();

    /**
      * create the inner panel on which strings are drawn
      */
    void createInnerScreen();

private:

    int m_left;             //!< upper left corner left offset in pixels from X-Plane main window

    int m_right;            //!< upper right corner right offset in pixels from X-Plane main window

    int m_top;              //!< upper left corner upper offset in pixels from X-Plane main window

    int m_bottom;           //!< lower left corner bottom offset in pixels from X-Plane main window

    std::string m_title;    //!< title bar caption

    std::string m_message;  //!< message to display

    bool m_quit_on_confirm; //!< quit X-Plane when user closes message box

    bool m_is_displayed;

    XPWidgetID m_box_widget;

    XPWidgetID m_screen_widget;

    std::vector<XPWidgetID> m_caption_widgets_list;

    DataRef<int> m_xp_width;   //!< X-Plane main window width

    DataRef<int> m_xp_height;  //!< X-Plane main window height
};

}

#endif // MESSAGEWINDOW_H
