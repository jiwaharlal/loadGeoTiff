set(Sources_List
    main.cpp
    MainView.cpp
    MainView.h
    MainWindow.cpp
    MainWindow.h
    TiffRenderer.cpp
    TiffRenderer.h
)

set(Moc_Headers
)

ListSources("Sources_List" "Sources_List_Full_Path")

set(Folders_List)

ListFolders("Folders_List")
