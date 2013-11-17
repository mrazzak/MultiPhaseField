% function dynamic_energy_well(param)
clear all

x=0.28; % initial condition.
xend=0.8; % ending potions of interface
value=5e-18;
pr=3
particles_number=1
figure;
% phase field parameters
L=5*[1 1]*1e18;
alpha=1*[1 1]*1e-18;
beta=1*[1 1]*1e-18;
gamma=1*1.5*[1 1]*1e-18;
kappa=0.5*[1 1]*1e-18;
epsilon=value;

DelG=[0 0.5]*1e-19;
% BCValue=value;
savedir='/home/cenna/Documents/Projects/PhD/Presentations/Modelling Seminar/axixsym_pinning'
% setings structure
settings.L=L(1);
settings.alpha=alpha(1);
settings.beta=beta(1);
settings.gamma=gamma(1);
settings.kappa=kappa(1);
settings.epsilon=epsilon(1);
settings.DelG=DelG;
settings.accuracy='low';
% geometry settings
p=2;
global nboxsize mboxsize
global delx delt scale
scale=3;
mboxsize=30*scale; % y axis in pixels
nboxsize=15*scale; % x axis
delx=2/scale;      % length unit per pixel

% Particle geometry
% pr=3;  % size in length unit
xparticle=0; % position on the grid (pixel)
yparticle=fix(mboxsize/2*10/10);

timestepn=2000;
delt=0.02;
%savesettings
% save(strcat(pwd,'/',savedir,'/','setings.mat'))

% *** Phase Field Procedure *** (so small and simple piece of code!)
eta=zeros(mboxsize,nboxsize,p);
% making initial structure
% eta(:,:,1)=circlegrain(mboxsize,nboxsize,nboxsize/2,4.5*mboxsize/10,grainD,'dome');
eta(:,:,1)=zeros(mboxsize,nboxsize);
x=fix(mboxsize*x);
eta(1:x,:,1)=1;
eta(:,:,2)=imcomplement(eta(:,:,1));
% ppf is phase variable representing particles
%particledistro(nboxsize,mboxsize,particles_number,radius)
[ppf]=particledistro(nboxsize,mboxsize,particles_number,pr/delx*2,0,yparticle);
% ppf2=ppf;
eta2=zeros(mboxsize,nboxsize,p); %pre-assignment
%initialization

%initialization
for tn=1:20
    for i=1:mboxsize
        for j=1:nboxsize
            del2=1/delx^2*(0.5*(eta(indg(i+1,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,mboxsize),j,:))...
                +0.25*(eta(indg(i+2,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-2,mboxsize),j,:)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,nboxsize),:))...
                +0.25*(eta(i,indg(j+2,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-2,nboxsize),:)))...
                +1/(j*delx)*(eta(i,indg(j+1,nboxsize))-eta(i,indg(j-1,nboxsize)));
            sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
            detadtM=(-alpha.*reshape(eta(i,j,:),1,p)+beta.*reshape(eta(i,j,:),1,p).^3-kappa.*reshape(del2,1,p)+...
                2*epsilon.*reshape(eta(i,j,:),1,p)*ppf(i,j))+DelG;
            detadt=-L.*(detadtM+2*gamma(1)*reshape(sumterm,1,p));
            eta2(i,j,:)=eta(i,j,:)+reshape(delt*detadt,1,1,2);
        end
    end
    eta=eta2;
    phi=sum(eta(:,:,1:p).^2,3);
    phi=phi+ppf;
%     [cur1,cur2,detach,cent1,cent2]=analysecontourgrains(eta,xparticle,yparticle,tn,ppf);
%     cent1p=cent1;
%     cent2p=cent2;
end
se=strel('square',6);
timevec=0;
tn=0;
xend=fix(mboxsize*xend);
while eta(xend,1,1)<0.5 % when interface arrives at xend
    Mdetadt=zeros(mboxsize,nboxsize);
    tn=tn+1;
    [yii,xjj]=find(...
        imerode((phi>0.9),se)==0);
    for ii=1:length(xjj)
        i=yii(ii);j=xjj(ii);
        del2=1/delx^2*(0.5*(eta(indg(i+1,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,mboxsize),j,:))...
            +0.25*(eta(indg(i+2,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-2,mboxsize),j,:)))...
            +1/delx^2*(0.5*(eta(i,indg(j+1,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,nboxsize),:))...
            +0.25*(eta(i,indg(j+2,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-2,nboxsize),:)))...
            +1/(j*delx)*(eta(i,indg(j+1,nboxsize))-eta(i,indg(j-1,nboxsize)));
        sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
        detadtM=(-alpha.*reshape(eta(i,j,:),1,p)+beta.*reshape(eta(i,j,:),1,p).^3-kappa.*reshape(del2,1,p)+...
            2*epsilon.*reshape(eta(i,j,:),1,p)*ppf(i,j))+DelG;
        detadt=-L.*(detadtM+2*gamma(1)*reshape(sumterm,1,p));
        % matrix of the eta.dot
        eta2(i,j,:)=eta(i,j,:)+reshape(delt*detadt,1,1,2);
        %             elovution of particle
        %             del2=1/delx^2*(0.5*(ppf(indg(i+1,gridn),j)-2*ppf(i,j)+ppf(indg(i-1,gridn),j))...
        %                 +0.25*(ppf(indg(i+2,gridn),j)-2*ppf(i,j)+ppf(indg(i-2,gridn),j)))...
        %                 +1/delx^2*(0.5*(ppf(i,indg(j+1,gridn))-2*ppf(i,j)+ppf(i,indg(j-1,gridn)))...
        %                 +0.25*(ppf(i,indg(j+2,gridn))-2*ppf(i,j)+ppf(i,indg(j-2,gridn))));
        %             sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
        %             detadtM=(-alpha.*reshape(eta(i,j,:),1,p)+beta.*reshape(eta(i,j,:),1,p).^3-kappa.*reshape(del2,1,p)+...
        %                 2*epsilon.*reshape(eta(i,j,:),1,p)*ppf(i,j))+G;
        %             detadt=-L.*(detadtM+2*gamma*reshape(sumterm,1,p));
        %             eta2(i,j,:)=eta(i,j,:)+reshape(delt*detadt,1,1,2);

%         Mdel2(i,j)=del2(2);
    end

%     [ppf2]=particledistro(nboxsize,mboxsize,1,pr/delx*2,xparticle,yparticle);
%     eta2(:,:,1)=boundarycond(eta2(:,:,1),ppf2,0);
%     eta2(:,:,2)=boundarycond(eta2(:,:,2),ppf2,0);


%     Mdetadt=(eta2(:,:,1)-eta(:,:,1))/delt;
    eta=eta2;

    % making optimization matrix
    [deletax1 deletay1]=gradient(eta(:,:,1),delx,delx);
    [deletax2 deletay2]=gradient(eta(:,:,2),delx,delx);
    phi=imcomplement(abs(deletax1)+abs(deletay1)+abs(deletax2)+abs(deletay2));
%     phi=sum(eta(:,:,1:p).^2,3);

    timevec(tn+1)=timevec(tn)+delt;
    %%% Visulaizations

    %% simple structure
    %   subplot(2,3,1)
    %
    % drawgrains(phi,xparticle,yparticle,tn)
    %     contourgrains(eta,xparticle,yparticle,tn,ppf)
    %% Energy plots
    %     [ME,E]=calculateE(eta,ppf,mboxsize,nboxsize,delx);
    % VecE(tn)=E;
    %     drawE(rot90(rot90(ME)),xparticle,yparticle,tn,eta,ppf)

    %% Middle point curvature and speed
%     [cur1,cur2,detach,cent1,cent2]=analysecontourgrains(eta,xparticle,yparticle,tn,ppf);
%     curi1(tn)=cur1;
%     curi2(tn)=cur2;
%     detachi(tn)=detach;
    %% speed
    %     speed1(tn)=(cent1-cent1p)/delt;
    %     speed2(tn)=(cent2-cent2p)/delt;
    %     cent1p=cent1;
    %     cent2p=cent2;
%     Mcent1(tn)=cent1;Mcent2(tn)=cent2;

    %
    %     %% Speed Plots
    %     subplot(2,3,[6])
    %     plot(timevec(2:end),speed1,timevec(2:end),speed2,'r');
    %     title(strcat('v_{\eta_1}= ', num2str(speed1(tn)),' , ',...
    %         'v_{\eta_2}= ', num2str(speed2(tn))));
    %     xlabel('time')
    %     %% Curvature Plots
    %     subplot(3,1,3)
    %     plot(timevec(2:end),curi1,timevec(2:end),curi2,'r');
    %         xlabel('time')
    %     title(strcat('\kappa_{\eta_1}= ', num2str(cur1),' , ',...
    %         '\kappa_{\eta_2}= ', num2str(cur2)));


    %% Velocity map
%     [nablaetax,nablaetay]=gradient(eta(:,:,1),delx,delx);
%     %     nablaetax((phi>0.99))=nan;
%     %     nablaetay((phi>0.99))=nan;
%     Mvelocx=Mdetadt./nablaetax;
%     Mvelocy=Mdetadt./nablaetay;
%     %         Mveloc=sqrt(Mvelocx.^2+Mvelocy.^2);
%     Mveloc=Mdetadt./sqrt(nablaetax.^2+nablaetay.^2);
    %
    %% Average velocity field over certain area of the domain:
%     [vel]=aveveloc(Mveloc,phi,ppf,Mdetadt,'dome');
%     Mvel(tn)=vel;
%     %     MD(tn)=D;
%     h=subplot(2,3,1);
%     plot(timevec(2:end),-Mvel)
%     title('Velocity average based on the field')
%     xlabel(strcat('Time= ', num2str(tn*delt)))

    %% plotting places where average velocity is calculated
%     maxM=0.9*max(max(abs(Mdetadt)));
%     h=subplot(2,3,6);
%     phivel=phi;
%     phivel(abs(Mdetadt)>maxM)=0;
%     imshow(phivel)

    %     h=subplot(2,3,2);
    %     drawvelocity(Mvelocx);
    %     set(h,'clim',[-0.5 0.5])
    %     title('X velocity component')
    %     %
    %     h=subplot(2,3,3);
    %     drawvelocity(Mvelocy)
    %     set(h,'clim',[-0.7 0.7])
    %     title('Y velocity component')
    %
%     h=subplot(2,3,2);
%     drawvelocity(Mveloc)
%     set(h,'clim',[-0.9 0])
%     title('Velocity Magnitude')
%     xlabel(strcat('Time= ', num2str(tn*delt)))
%     %
    %     h=subplot(2,3,6);
    %     drawvelocity(nablaetax)
    %     set(h,'clim',[-0.3 0.3])
    %     title('\nabla \eta x')
    %
    %     h=subplot(2,3,4);
    %     drawvelocity(nablaetay)
    %     set(h,'clim',[-0.3 0.3])
    %     title('\nabla \eta y')
    %
    %     h=subplot(2,3,1);
    %     drawvelocity(Mdetadt)
    %     set(h,'clim',[-0.2 0])
    %     title('d\eta / dt')

    %% Force on a circular region inside the domain
%     [F,Fprofile,ME,xp,yp]=Force(eta,ppf,setings,pr,xparticle,yparticle);
%     Mforce(:,tn)=F';
%     h=subplot(2,3,4);
%     plot(timevec(2:end),Mforce(2,:))
%     hold on
%     plot(timevec(2:end),Mforce(1,:),'r')
%     title('Force acting on circular region, red=Fx, blue=Fy')
%     xlabel(strcat('Time= ', num2str(tn*delt)))
%     hold off

    %% Draw Energy Field
%    h=subplot(2,1,2);
    [ME,E]=calculateE_axisym(eta,ppf,mboxsize,nboxsize,delx,settings);
%     drawvelocity(ME)
    MME(tn)=E;
%     title(['Energy density at timestep' num2str(tn)])
    % %     set(h,'clim',[-0.2 0])



    %% Volume of each phase field
%     etaVol=etaVolume(eta,delx,nboxsize,mboxsize);
%     MetaVol(:,tn)=etaVol';
%     h=subplot(2,3,5);
%     plot(timevec(2:end),MetaVol(1,:)/(mboxsize*nboxsize*delx^2))
%     hold on
%     plot(timevec(2:end),MetaVol(2,:)/(mboxsize*nboxsize*delx^2),'r')
%     plot(timevec(2:end),(MetaVol(1,:)+MetaVol(2,:))/(mboxsize*nboxsize*delx^2),'g')
%     title('Volume fraction of phases')
%     xlabel(strcat('Time= ', num2str(tn*delt)))
%     hold off

% %% Draw del2 Field
%    h=subplot(2,1,1);
%   
%     drawvelocity(Mdel2)
% 
%     % %     set(h,'clim',[-0.2 0])
%     title(['\nabla^2 at timestep' num2str(tn)])


    %   savegrains(eta,ppf,E,xparticle,yparticle,tn,savedir)

    % additional expriments
    %     MMdetadt(:,:,tn)=Mdetadt;
    %     MMeta1(:,:,tn)=eta(:,:,1);
    %
    %% SAVING

    %  save(strcat(pwd,'/',savedir,'/',num2str(tn),'.mat'),...
    %      'Mdetadt','eta','Mveloc','Mvelocx','Mvelocy','tn')
   pause(0.01)
    %     disp(strcat('Time= ', num2str(tn*delt)))
%     tn
    
    %% find position of interface on the corner
    intindex=find(eta(:,end,1)>0.01 & eta(:,end,1)<0.99);
    intpos=interp1(eta(intindex,end,1),intindex*delx,0.5,'spline');
    Mintpos(tn)=intpos;
    if mod(tn,80)==0
         intpos
         drawvelocity_axisym(ME)
         filename=strcat(savedir,'/',num2str(tn),'.png');
         print('-f1','-dpng','-r100',filename)
    end
end

% % plotting profiles of energy order parameters
% figure
% y=[1:mboxsize]*delx-mboxsize*delx/2;
% subplot(2,1,1)
% plot(y,eta(:,fix(nboxsize/2),1))
% hold on
% plot(y,eta(:,fix(nboxsize/2),2),'r')
% ylabel('Ordere Parameter')
% plot(y,ppf(:,fix(nboxsize/2)),'g')
% 
% plot(mboxsize/2*delx*[0 0],[0 1],'k')
% 
% subplot(2,1,2)
% plot(y,ME(:,fix(nboxsize/2)),'r')
% xlabel('Position (X unit)')
% ylabel('Energy Density')
% hold on
% plot(mboxsize/2*delx*[0 0],[0 0.8e-18],'k')

% Sigma=E/(nboxsize*delx)*1e18


%% calculating potential well
% figure
% plot(timevec(2:end),MME)

figure
plot(Mintpos,MME)
hold on
%fitting two straight lines on energy curve
ln1ind=find(Mintpos>0 & Mintpos<22);
plot(Mintpos(ln1ind),MME(ln1ind),'y')
pp1=polyfit(Mintpos(ln1ind),MME(ln1ind),1);

ln2ind=find(Mintpos>46);
plot(Mintpos(ln2ind),MME(ln2ind),'g')
pp2=polyfit(Mintpos(ln2ind),MME(ln2ind),1);

% potential well re-construction

well=MME-polyval(pp1,Mintpos);
figure
plot(Mintpos,well)


