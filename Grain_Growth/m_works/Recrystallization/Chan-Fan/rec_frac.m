function [areaG,diamG,perimG,L]=rec_frac(phi)
% For making white regions brighter
phi(phi>1)=nan;
graymap=uint8(255/(max(max(phi-min(min(phi)))))*(phi-(min(min(phi)))));
%         subplot(2,1,1) 
%         imshow(imadjust(imresize(uint8(graymap),mag),[10 255]./255,[]));
%         subplot(2,1,2)
%         plot(reshape(eta(:,4,:),gridn,p))
graymap=imadjust(graymap,[120 255]./255,[]);
grayind=graythresh(graymap);
BW=im2bw(graymap,grayind);
BW=imfill(BW,'holes');
BW=bwmorph(BW,'thicken',2);
BW=imcomplement(bwmorph(imcomplement(BW),'thin',inf));
BW=imcomplement(bwmorph(imcomplement(BW),'spur',inf));
BW=imclearborder(BW,4);

% 
% figure
% hold on
% plot(mag*xparticle,mag*yparticle,'r.','MarkerSize',4)
% hold off
% imshow(BW)
% pause(1)
% 
