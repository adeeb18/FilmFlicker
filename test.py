#!/usr/bin/env python

import glob
import wx
from subprocess import Popen, PIPE, STDOUT
p = Popen(['COP3503Project3.exe'], stdin = PIPE, stderr = PIPE)

class FilmsPanel(wx.Panel):
    def __init__(self, parent):
        super().__init__(parent)
        main_sizer = wx.BoxSizer(wx.VERTICAL)

        self.row_obj_dict = {}

        self.list_ctrl = wx.ListCtrl(
            self, size=(-1, 500),
            style=wx.LC_REPORT | wx.BORDER_SUNKEN
        )

        st = wx.StaticText(self, label = "Welcome to FilmFlicker! Input a movie to see movie suggestions", pos = (370, 0))
        font = st.GetFont()
        font.PointSize = 15
        st.SetFont(font)
        self.text_ctrl = wx.TextCtrl(self)
        main_sizer.Add(self.text_ctrl, 0, wx.ALL | wx.EXPAND, 40)
        my_button = wx.Button(self, label='Calculate')
        my_button.Bind(wx.EVT_BUTTON, self.button_press)
        button_2 = wx.Button(self, label = "Display Performance Info")
        button_2.Bind(wx.EVT_BUTTON, self.button_press2)
        clear_button = wx.Button(self, label = "Clear")
        clear_button.Bind(wx.EVT_BUTTON, self.button_press_clear)
        main_sizer.Add(my_button, 0, wx.ALL | wx.CENTER, -25)
        main_sizer.Add(button_2, 0, wx.ALL | wx.CENTER, 30)
        main_sizer.Add(clear_button, 0, wx.ALL | wx.CENTER, -20)
        main_sizer.Add(self.list_ctrl, 0, wx.ALL | wx.CENTER, 30)
        self.SetSizer(main_sizer)
        
        
    def button_press(self, event):
        p.communicate(input=self.text_ctrl.GetValue().encode('utf-8'))
        self.films = dict()
        with open("MovieFile.txt") as file:
            movie_list = file.readlines()
            for x, line in enumerate(movie_list):
                self.films[x] = wx.StaticText(self, label = line, pos = (550, 20*x + 250))

    def button_press2(self, event):
        with open("TimeFile.txt") as file:
            info = file.readlines()
            line = (info[0])
            line1 = (info[1])
            self.label1 = wx.StaticText(self, label = "Depth-First Search: ", pos = (510, 675))
            self.label2 = wx.StaticText(self, label = "Breadth-First Search: ", pos = (510, 695))
            self.dfs = wx.StaticText(self, label = line, pos = (640, 675))
            self.bfs = wx.StaticText(self, label = line1, pos = (640, 695))
            self.units1 = wx.StaticText(self, label = " seconds ", pos = (700, 675))
            self.units2 = wx.StaticText(self, label = " seconds ", pos = (700, 695))

    def button_press_clear(self, event):
        for key in self.films:
            self.films[key].Destroy()
        self.label1.Destroy()
        self.label2.Destroy()
        self.dfs.Destroy()
        self.bfs.Destroy()
        self.units1.Destroy()
        self.units2.Destroy()
            

class MyFrame(wx.Frame):    
    def __init__(self):
        super().__init__(parent=None, title='FilmFlicker')
        size = (1300, 800)
        wx.Window.SetSize(self, size)
        panel = FilmsPanel(self)
        panel.SetBackgroundColour('#89CFF0')

        self.Show()

    
if __name__ == '__main__':
    app = wx.App()
    frame = MyFrame()
    app.MainLoop()
