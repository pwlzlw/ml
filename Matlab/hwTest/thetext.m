function thetext( str) 

    royalb = 1/256*[65,105,225];
    royalr = 1/255*[235,43,54];
    royalg = 1/255*[0,104,87];
    colorsR = {royalb;royalr;royalg};
    
    hFigure = figure('MenuBar', 'none', ...
                 'ToolBar', 'none');

    hText = uicontrol('Parent', hFigure, ...  %# Create a text object
                      'Style', 'text', ...
                      'String', str, ...
                      'BackgroundColor', 'w', ...
                      'ForegroundColor', royalr, ...
                      'FontSize', 36, ...
                      'FontWeight', 'bold');

    set([hText hFigure], 'Pos', get(hText, 'Extent'));  %# Adjust the sizes of the
                                                        %#   text and figure
    imageData = getframe(hFigure);  %# Save the figure as an image frame
    delete(hFigure);

    textImage = imageData.cdata;  %# Get the RGB image of the text

    surf([0 1; 0 1], [1 0; 1 0], [1 1; 0 0], ...
         'FaceColor', 'texturemap', 'CData', textImage);
    set(gca, 'Projection', 'perspective', 'CameraViewAngle', 45, ...
        'CameraPosition', [0.5 -1 0.5], 'Visible', 'off');
end