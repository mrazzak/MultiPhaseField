%%% This is Multi Phase Field simulation for normal grain growth in 2D [1,2].
%%% The particle pinning based on Molean's functional [3] is also included.
%
% References:
% 1. L-Q Chen, W. Yang, Phys. Rev. B, 1994, 50; 15752.
% 2. D. Fan, L. Q. Chen, Acta Mater. 1997, 45; 611.
% 3. N. Moelans, B. Blanpain, P. Wollants, Acta Mater. 2005, 54;1175.
%

clear
savedir='test';
% choose whether save or not
saveresults=1;
% time steps that save command is executed
savetimesteps=[1:100 105:5:1000 1020:20:4000 4050:50:10000 10100:100:20000 20200:200:40000];
mkdir(savedir)
figure;
% clf
% phase field parameters
L=1;
alpha=1;
beta=1;
gamma=1;
kappa=2;
epsilon=5;
% geometry settings
p=30; % phase field numbers
mboxsize=600; % system grid numbers in x direction
nboxsize=600; % system grid numbers in y direction
delx=2;       % Delta x

% time discritization. The maximum delta t should not be very larger and
% depends on the value of the delx to maintain the stability of
% calculation
delt=[linspace(0.25,0.25,100) linspace(0.25,0.3,900) linspace(0.3,0.3,3000)...
    linspace(0.3,0.3,6000) linspace(0.3,0.4,10000) linspace(0.4,0.45,20000)];

timestepn=size(delt,2);
% number of nucleas at the beginning of simulation
nuclein=mboxsize*nboxsize/20; % ~5 percent of grid points are nuclei
% particles distribution specification
diameter=1;
particles_fraction=0;
% particles number
particlesn=particles_fraction*nboxsize*nboxsize/diameter^2

%% for making non-uniform distribution of the particles use particledistroN
% ratio of banded phase fraction to uniform distribution outside band
%ratio=50;

% *** Phase Field Procedure *** (so small and simple piece of code!)
eta=sparse(nboxsize,mboxsize*p);
% putting initial nucleas 1:total number of nuclea (so nucleation density
% can be set)
% One can use non-uniform distribution of nuclei to have bimodal structure
for nn=1:nuclein;
    ii=fix(nboxsize*rand(1,1))+1;jj=fix(mboxsize*p*rand(1,1))+1;
    eta(ii,jj)=1;
end
eta(:,1:mboxsize)=0;
for pn=0:p-1
    eta(251:350,251+mboxsize*pn:350+mboxsize*pn)=...
        eta(251:350,251+mboxsize*pn:350+mboxsize*pn).*imcomplement(imcircle(100));
end
eta(261:340,261:340)=imcircle(80);

%% Particles distribution. ppf is phase variable representing particles

% particledistro(nboxsize,mboxsize,particles_number,radius) if particles
% number is zero no particles in there and ppf is just zero matrix
[ppf,xparticle,yparticle]=particledistro(nboxsize,mboxsize,particlesn,diameter);

% non-uniform particles distribution in a banded structure
% [ppf,xparticle,yparticle]=particledistroN(nboxsize,mboxsize,particles_fraction,diameter,ratio);

% This is another method to make intial seeds (Used by L.Q. Chen). There
% is no control over grain distribution in this method
% eta=rand(gridn,gridn,p)*0.001;

% save settings and initial conditions
save(strcat(pwd,'/',savedir,'/','setings.mat'))
% eta 2 holds phase field parameters in a time step i+1
eta2=sparse(nboxsize,mboxsize*p); %pre-assignment
Timehistory=zeros(1,timestepn+1); % pre-assignment timehistory
%% first initial loops to make a interface a little bit thicker and making
% phi matrix for finding changing areas for optimized calculation method.
% First loops doesn't have image processing part so one can compare speed
% of the calculation. The less grain boundaries the faster the calculations
pn=0:p-1;
phi=sparse(nboxsize,mboxsize);
for tn=1:10
    tic
    for i=1:mboxsize
        for j=1:nboxsize
            % calculation of nabla squared eta
            del2=1/delx^2*(0.5*(eta(indg(i+1,nboxsize),j+pn*mboxsize)-2*eta(i,j+pn*mboxsize)+eta(indg(i-1,nboxsize),j+pn*mboxsize))...
                +0.25*(eta(indg(i+2,nboxsize),j+pn*mboxsize)-2*eta(i,j+pn*mboxsize)+eta(indg(i-2,nboxsize),j+pn*mboxsize)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,mboxsize)+pn*mboxsize)-2*eta(i,j+pn*mboxsize)+eta(i,indg(j-1,mboxsize)+pn*mboxsize))...
                +0.25*(eta(i,indg(j+2,mboxsize)+pn*mboxsize)-2*eta(i,j+pn*mboxsize)+eta(i,indg(j-2,mboxsize)+pn*mboxsize)));
            % double summation part in the PDE equation. cube term is to
            % substract the etai*eta^2 part from sum and get the j~=i
            % summation
            sumterm=eta(i,j+pn*mboxsize)*sum(eta(i,j+pn*mboxsize).^2)-eta(i,j+pn*mboxsize).^3;
            detadtM=(-alpha*eta(i,j+pn*mboxsize)+beta*eta(i,j+pn*mboxsize).^3-kappa*del2+...
                2*epsilon*eta(i,j+pn*mboxsize)*ppf(i,j));
            detadt=-L*(detadtM+2*gamma*(sumterm));
            eta2(i,j+pn*mboxsize)=eta(i,j+pn*mboxsize)+delt(tn)*detadt;
            % for making sure eta is not outside the equilibrium values
            % actually it is unnecessary

            for pind=0:p-1
                if eta2(i,j+pind*mboxsize)>1
                    eta2(i,j+pind*mboxsize)=1;
                end
                if eta2(i,j+pind*mboxsize)<1e-6
                    eta2(i,j+pind*mboxsize)=0;
                end
            phi=phi+eta(:,1+pind*mboxsize:mboxsize+pind*mboxsize).^2;
            end
        end
    end
    eta=eta2;

    % for making particle positions 1
    %     phi=phi+ppf;
    drawgrains(phi,ppf,xparticle,yparticle,tn)
    toc
end
% strel element for the kernel of imerode
se=strel('square',3);
% time discretization loop
tn=0;
pn=0:p-1;
while tn<timestepn
    tn=tn+1;
    tic
    % findig nodes which are in the grain boundaries and solve
    % differential equation only for that points.
    [yii,xjj]=find(...
        imerode((phi>0.999),se)==0);
    % space discretization loop
    for ii=1:length(xjj)
        i=yii(ii);j=xjj(ii);
        % finding etas that are not zero
        etaind=find((eta(i,j+pn*mboxsize)>0.0001));
        del2=zeros(1,p);
        sumterm=del2;
        detadtM=del2;
        detadt=del2;
        j=j+etaind*mboxsize;
        del2(1,:)=1/delx^2*(0.5*(eta(indg(i+1,nboxsize),j)-2*eta(i,j)+eta(indg(i-1,nboxsize),j))...
            +0.25*(eta(indg(i+2,nboxsize),j,etaind)-2*eta(i,j,etaind)+eta(indg(i-2,nboxsize),j)))...
            +1/delx^2*(0.5*(eta(i,indg(j+1,mboxsize),etaind)-2*eta(i,j,etaind)+eta(i,indg(j-1,mboxsize)))...
            +0.25*(eta(i,indg(j+2,mboxsize),etaind)-2*eta(i,j,etaind)+eta(i,indg(j-2,mboxsize),etaind)));
        sumterm=eta(i,j,etaind)*sum(eta(i,j,etaind).^2)-eta(i,j,etaind).^3;
        detadtM=(-alpha*eta(i,j,etaind)+beta*eta(i,j,etaind).^3-kappa*del2+...
            2*epsilon*eta(i,j,etaind)*ppf(i,j));
        detadt=-L*(detadtM+2*gamma*(sumterm));
        eta2(i,j,etaind)=eta(i,j,etaind)+delt(tn)*detadt;
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
    %% saving the structure
    % desired saving time steps
    if ~isempty(find(savetimesteps==tn)) && saveresults==1
     filename=strcat(pwd,'/',savedir,'/',num2str(tn),'.mat');
     save(filename,'phi','tn','eta') % one can add eta, for having all phase parameters
    end
    %     savegrains(phi,xparticle,yparticle,tn,savedir)
    % storing time history vector for the case where delt varies
    Timehistory(tn+1)=Timehistory(tn)+delt(tn);
    % display speed of this step calulation. Using
    toc
    pause(0.1)
end
%


%% calculation over all the field parameters

% 
%     tn=tn+1;
%     tic
%     % findig nodes which are in the grain boundaries and solve
%     % differential equation only for that points.
%     [yii,xjj]=find(...
%         imerode((phi>0.999),se)==0);
%     % space discretization loop
%     for ii=1:length(xjj)
%         i=yii(ii);j=xjj(ii);
%         del2=1/delx^2*(0.5*(eta(indg(i+1,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,nboxsize),j,:))...
%             +0.25*(eta(indg(i+2,nboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-2,nboxsize),j,:)))...
%             +1/delx^2*(0.5*(eta(i,indg(j+1,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,mboxsize),:))...
%             +0.25*(eta(i,indg(j+2,mboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-2,mboxsize),:)));
%         sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
%         detadtM=(-alpha*eta(i,j,:)+beta*eta(i,j,:).^3-kappa*del2+...
%             2*epsilon*eta(i,j,:)*ppf(i,j));
%         detadt=-L*(detadtM+2*gamma*(sumterm));
%         eta2(i,j,:)=eta(i,j,:)+delt(tn)*detadt;
%         for pind=1:p
%             if eta2(i,j,pind)>1
%                 eta2(i,j,pind)=1;
%             end
%             if eta2(i,j,pind)<0
%                 eta2(i,j,pind)=0;
%             end
%         end
%     end
% 
%     eta=eta2;
%     phi=sum(eta(:,:,1:p).^2,3);
%     % adding ppf to the phi to make particles positions to 1 inorder to
%     % make mapping more clear to see. Because the range of phi changes
%     % from 0.5 to 1 and having zero element makes range broader and
%     % lower contrast. It dosen't do anything with the eta matrix
% %     phi=phi+ppf;
%     % draw gray scale image of structure. with particles in red dots
%     drawgrains(phi,ppf,xparticle,yparticle,tn)
%     %% saving the structure
%     % desired saving time steps
% 
% 
%     % display speed of this step calulation. Using
%     toc
% 
%     
