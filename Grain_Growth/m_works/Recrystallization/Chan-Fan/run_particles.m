function fraction=run_particles(param,savedir)
%%% This is Multi Phase Field simulation for normal grain growth in 2D [1,2].
%%% The particle pinning based on Molean's functional [3] is also included.
%
% References:
% 1. L-Q Chen, W. Yang, Phys. Rev. B, 1994, 50; 15752.
% 2. D. Fan, L. Q. Chen, Acta Mater. 1997, 45; 611.
% 3. N. Moelans, B. Blanpain, P. Wollants, Acta Mater. 2005, 54;1175.
%
% function realtime_particles()
% clear
% savedir='c:\PF_Data\test_guillaume\no_band';
% choose whether save or not
saveresults=1;
% time steps that save command is executed
savetimesteps=[1:2:100 105:5:1000 1020:20:4000 4050:50:10000 10100:100:20000 20200:200:40000];
savedetaile=[50 100:50:500 600:100:3000 3000:200:4000 4500:500:10000 11000:1000:40000];
mkdir(savedir)
% figure;
% clf
% phase field parameters
L=1;
alpha=1 ;
beta=1 ;
gamma=1.5;
kappa=2;
epsilon=5;


setings.L=L;
settings.alpha=alpha;
settings.beta=beta;
settings.gamma=gamma;
settings.kappa=kappa;
settings.epsilon=epsilon;


% geometry settings
p=40; % phase field numbers
scale=1;
mboxsize=250*scale; % x axis in pixels
nboxsize=250*scale; % y axis
delx=2/scale;      % length unit per pixel

% time discritization. The maximum delta t should not be very larger and
% depends on the value of the delx to maintain the stability of
% calculation
delt=[linspace(0.25,0.25,100) linspace(0.25,0.25,1000)];

timestepn=size(delt,2);
% number of nucleas at the beginning of simulation
nuclein= mboxsize*nboxsize/200; % ~0.5 percent of grid points are nuclei

% particles distribution specification
diameter=2;
%% particles number
% particles_fraction=0.00;
% particlesn=particles_fraction*nboxsize*nboxsize/diameter^2
particlesn=fix(param);
%% for making non-uniform distribution of the particles use particledistroN
% ratio of banded phase fraction to uniform distribution outside band
%ratio=50;

% *** Phase Field Procedure *** (so small and simple piece of code!)
eta=zeros(nboxsize,mboxsize,p);
% putting initial nucleas 1:total number of nuclea (so nucleation density
% can be set)
% One can use non-uniform distribution of nuclei to have bimodal structure
for nn=1:nuclein;
    ii=fix(nboxsize*rand(1,1))+1;jj=fix(mboxsize*rand(1,1))+1;
    eta(ii,jj,fix(p*rand(1,1))+1)=1;
end

% remove a band of nuclei
eta(fix(nboxsize/2-1/10*nboxsize):fix(nboxsize/2+1/10*nboxsize),:,:)=0;
%% add one big grain code here
%
% eta(:,:,1)=0;
% Bigcirc=108*scale;
% Bigcirc2=100*scale;
% for pn=1:p
%     eta(nboxsize/2-Bigcirc/2+1:nboxsize/2+Bigcirc/2,mboxsize/2-Bigcirc/2+1:mboxsize/2+Bigcirc/2,pn)=...
%         eta(nboxsize/2-Bigcirc/2+1:nboxsize/2+Bigcirc/2,mboxsize/2-Bigcirc/2+1:mboxsize/2+Bigcirc/2,pn)...
%         .*imcomplement(imcircle(Bigcirc));
% end
% eta(nboxsize/2-Bigcirc2/2+1:nboxsize/2+Bigcirc2/2,mboxsize/2-Bigcirc2/2+1:mboxsize/2+Bigcirc2/2,1)=imcircle(Bigcirc2);

%% Particles distribution. ppf is phase variable representing particles
% particledistro(nboxsize,mboxsize,particles_number,radius) if particles
% number is zero no particles in there and ppf is just zero matrix
[ppf,xparticle,yparticle]=particledistro(nboxsize,mboxsize,particlesn,diameter,'band');

%% another distribution of particles
% diameter=1;
% particles_fraction=0.005;
% % particles number
% particlesn=particles_fraction*nboxsize*nboxsize/diameter^2
% [ppf2,xparticle2,yparticle2]=particledistro(nboxsize,mboxsize,particlesn,diameter,'uniform');
%
% ppf=ppf+ppf2;
% xparticle=[xparticle xparticle2];
% yparticle=[yparticle yparticle2];

% non-uniform particles distribution in a banded structure
% [ppf,xparticle,yparticle]=particledistroN(nboxsize,mboxsize,particles_fraction,diameter,ratio);

% This is another method to make intial seeds (Used by L.Q. Chen). There
% is no control over grain distribution in this method:
% eta=rand(gridn,gridn,p)*0.001;

% save settings and initial conditions
save(strcat(savedir,'settings.mat'))
% eta 2 holds phase field parameters in a time step i+1
eta2=zeros(nboxsize,mboxsize,p); %pre-assignment
%% first initial loops to make a interface a little bit thicker and making
% phi matrix for finding changing areas for optimized calculation method.
% First loops doesn't have image processing part so one can compare speed
% of the calculation. The less grain boundaries the faster the calculations
Timehistory=0;
for tn=1:10
    tic
    for i=1:mboxsize
        for j=1:nboxsize
            % calculation of nabla square eta
            del2=1/delx^2*(0.5*(eta(indg(i+1,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,nboxsize),j,:))...
                +0.25*(eta(indg(i+2,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-2,nboxsize),j,:)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,mboxsize),:))...
                +0.25*(eta(i,indg(j+2,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-2,mboxsize),:)));
            % double summation part in the PDE equation. cube term is to
            % substract the etai*eta^2 part from sum and get the j~=i
            % summation
            sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
            detadtM=(-alpha*eta(i,j,:)+beta*eta(i,j,:).^3-kappa*del2+...
                2*epsilon*eta(i,j,:)*ppf(i,j));
            detadt=-L*(detadtM+2*gamma*(sumterm));
            eta2(i,j,:)=eta(i,j,:)+delt(tn)*detadt;
            % for making sure eta is not outside the equilibrium values
            % actually it is unnecessary
            for pind=1:p
                if eta2(i,j,pind)>1
                    eta2(i,j,pind)=1;
                end
                if eta2(i,j,pind)<0
                    eta2(i,j,pind)=0;
                end
            end
            
        end
    end
    eta=eta2;
    phi=sum(eta(:,:,1:p).^2,3);
    % for making particle positions 1
    %     phi=phi+ppf;
    drawgrains(phi,ppf,xparticle,yparticle,tn)
    if mod(tn,10)==0
        imagesave=[savedir num2str(tn) '.png']
        print('-f1','-dpng','-r200',imagesave)
    end
    
    %% saving the structure
    % desired saving time steps
    if ~isempty(find(savetimesteps==tn)) && saveresults==1
        filename=strcat(savedir,num2str(tn),'.mat')
        save(filename,'phi','tn') % one can add eta, for having all phase parameters
    end
    if ~isempty(find(savedetaile==tn)) && saveresults==1
        filename=strcat(savedir,'D',num2str(tn),'.mat');
        save(filename,'phi','tn','eta','Timehistory') % one can add eta, for having all phase parameters
    end
    %     savegrains(phi,xparticle,yparticle,tn,savedir)
    % storing time history vector for the case where delt varies
    Timehistory(tn+1)=Timehistory(tn)+delt(tn);
    % display speed of this step calulation. Using
    toc
    pause(0.1)
    %% calculating recrystallization fraction
    
    sf=sum(eta,3)+ppf;
    indsf=find(sf<0.5);
    fraction(tn)=length(indsf)/mboxsize/nboxsize;
end
% strel element for the kernel of imerode
se=strel('square',3);
% time discretization loop
while fraction>0.001 & tn<1000
    tn=tn+1
    tic
    % findig nodes which are in the grain boundaries and solve
    % differential equation only for that points.
    [yii,xjj]=find(...
        imerode((phi>0.999),se)==0);
    % space discretization loop
    for ii=1:length(xjj)
        i=yii(ii);j=xjj(ii);
        % calculation of nabla square eta
        del2=1/delx^2*(0.5*(eta(indg(i+1,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,nboxsize),j,:))...
            +0.25*(eta(indg(i+2,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-2,nboxsize),j,:)))...
            +1/delx^2*(0.5*(eta(i,indg(j+1,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,mboxsize),:))...
            +0.25*(eta(i,indg(j+2,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-2,mboxsize),:)));
        % double summation part in the PDE equation. cube term is to
        % substract the etai*eta^2 part from sum and get the j~=i
        % summation
        sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
        detadtM=(-alpha*eta(i,j,:)+beta*eta(i,j,:).^3-kappa*del2+...
            2*epsilon*eta(i,j,:)*ppf(i,j));
        detadt=-L*(detadtM+2*gamma*(sumterm));
        eta2(i,j,:)=eta(i,j,:)+delt(tn)*detadt;
        % for making sure eta is not outside the equilibrium values
        % actually it is unnecessary
        for pind=1:p
            if eta2(i,j,pind)>1
                eta2(i,j,pind)=1;
            end
            if eta2(i,j,pind)<0
                eta2(i,j,pind)=0;
            end
        end
    end
    
    eta=eta2;
    phi=sum(eta(:,:,1:p).^2,3);
    % adding ppf to the phi to make particles positions to 1 inorder to
    % make mapping more clear to see. Because the range of phi changes
    % from 0.5 to 1 and having zero element makes range broader and
    % lower contrast. It dosen't do anything with the eta matrix
    %     phi=phi+ppf;
    % draw gray scale image of structure. with particles in red dots
    drawgrains(phi,ppf,xparticle,yparticle,tn)
    if mod(tn,10)==0
        imagesave=[savedir num2str(tn) '.png']
        print('-f1','-dpng','-r200',imagesave)
    end
    %% saving the structure
    % desired saving time steps
    if ~isempty(find(savetimesteps==tn)) && saveresults==1
        filename=strcat(savedir,num2str(tn),'.mat');
        save(filename,'phi','tn') % one can add eta, for having all phase parameters
    end
    if ~isempty(find(savedetaile==tn)) && saveresults==1
        filename=strcat(savedir,'D',num2str(tn),'.mat');
        save(filename,'phi','tn','eta','Timehistory') % one can add eta, for having all phase parameters
    end
    %     savegrains(phi,xparticle,yparticle,tn,savedir)
    % storing time history vector for the case where delt varies
    Timehistory(tn+1)=Timehistory(tn)+delt(tn);
    % display speed of this step calulation. Using
    toc
    pause(0.1)
    %% calculating recrystallization fraction
    
    sf=sum(eta,3)+ppf;
    indsf=find(sf<0.5);
    fraction(tn)=length(indsf)/mboxsize/nboxsize;
end
filename=strcat(savedir,'final.mat');
save(filename)
%
%
% figure
% t=Timehistory(1:length(fraction));
% f=1-fraction;
% plot(t,f)
%
% % JMAK plot
% figure
% plot(log(t),log(log(1./(1-f))))


%% ----------------------------------------------------------
%Dependent functions

