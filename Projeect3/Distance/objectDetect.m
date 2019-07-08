function varargout = objectDetect(varargin)
% OBJECTDETECT M-file for objectDetect.fig
%      OBJECTDETECT, by itself, creates a new OBJECTDETECT or raises the existing
%      singleton*.
%
%      H = OBJECTDETECT returns the handle to a new OBJECTDETECT or the handle to
%      the existing singleton*.
%
%      OBJECTDETECT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in OBJECTDETECT.M with the given input arguments.
%
%      OBJECTDETECT('Property','Value',...) creates a new OBJECTDETECT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before objectDetect_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to objectDetect_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help objectDetect

% Last Modified by GUIDE v2.5 16-May-2011 03:42:59

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @objectDetect_OpeningFcn, ...
                   'gui_OutputFcn',  @objectDetect_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before objectDetect is made visible.
function objectDetect_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to objectDetect (see VARARGIN)

% Choose default command line output for objectDetect
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes objectDetect wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = objectDetect_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double


% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


load Distance.dat;
PC = Distance;
rect= imread('Img1r.jpeg');
disparity = reshape(PC(:,4),352,288)'; 
x=reshape(PC(:,1),352,288)';
y=reshape(PC(:,2),352,288)';
z=reshape(PC(:,3),352,288)';

%disparity
%find all pixels that are above a minimum disparity (relating to the 
%maximum distance 
imagebw=disparity >1;

%find the connected components 
conn=bwconncomp(imagebw,4);
conn2=bwlabel(imagebw,4);

area=zeros(1,conn.NumObjects); 
  
%find the size of each component 
for i=1:conn.NumObjects 
    %%area(i)=bwarea(L==i); 
    a=conn.PixelIdxList(i); 
    area(i)=size(a{1,1},1); 
end 
  
sarea = sort(area,'descend'); 

%keep objects that are over a particular size 
objects = find(area>1000); 

  
M=zeros(101376,1); 
  
%put only components over a certain size in a filter matrix to apply to the 
%rectified image and point cloud models 
for i=1:size(objects,2) 
    %M=M|(L==objects(i)); 
    a=conn.PixelIdxList(objects(i)); 
        M(a{1,1})=1;    
       
end 


M = reshape(M,288,352); 

%apply connected components filter to remove smaller unwanted components 
conn3=conn2.*M; 

%apply filter to x,y,z point cloud data 
X=x.*M; 
Y=y.*M; 
Z=z.*M; 
  
%apply extracted component filter to rectified image 
rect2=im2double(rect).*M; 

axes(handles.axes1)
imshow(rect2); 

axes(handles.axes2)
imshow(rect); 


Dx=X; 
Dy=Y; 
Dz=Z; 
  
X=reshape(X,size(X,1)*size(X,2),1); 
Y=reshape(Y,size(Y,1)*size(Y,2),1); 
Z=reshape(Z,size(Z,1)*size(Z,2),1); 

for i=1:size(objects,2) 
    a=conn.PixelIdxList(objects(i)); 
    xdist(i) = abs(max(X(a{1,1})))+abs(min(X(a{1,1}))); 
    ydist(i) = abs(max(Y(a{1,1})))+abs(min(Y(a{1,1}))); 
    zdist(i) = abs(max(Z(a{1,1})))+abs(min(Z(a{1,1}))); 
end 
  
dist(1,:)=xdist; 
dist(2,:)=ydist; 
dist(3,:)=zdist; 
%dist 
 
%save new point cloud with extracted objects shown 
X((X==0))=[]; 
Y((Y==0))=[]; 
Z((Z==0))=[]; 
Points=zeros(size(X,1),3); 
Points(:,1)=X; 
Points(:,2)=Y; 
Points(:,3)=Z; 
save('objects.txt','Points','-ASCII'); 
  
axes(handles.axes3) 
imagesc(conn3);

while(1)  
    
[x y] = ginput(1); 
D(1)=Dx(uint32(y),uint32(x)); 
D(2)=Dy(uint32(y),uint32(x)); 
D(3)=Dz(uint32(y),uint32(x)); 
set(handles.text3,'String',D(3));
end  
