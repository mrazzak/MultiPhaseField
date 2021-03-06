%% this is for straigth line diffuse interfacer with 1 phase field. and a
%% particle in the middle the exp term is also added for the test
% function [eta,ppf,ME,E]=stationarymin_p1(x)
% savedir='onefield';
% mkdir(savedir)
% figure;
% clf
x=0.5
%% phase field parameters
epsilon=5
L=[1];
alpha=[1];
beta=[1];
gamma=1;
kappa=[2];

% setings structure
setings.L=L;
setings.alpha=alpha;
setings.beta=beta;
setings.gamma=gamma;
setings.kappa=kappa;
setings.epsilon=epsilon;


minE=1/4*alpha*(2*beta-alpha)/beta^2;
% epsilon=10;

% geometry settings
p=1;
global nboxsize mboxsize
global delx
scale=5;
mboxsize=20*scale;
nboxsize=10*scale;
delx=2/scale;
grainD=0.5*scale;

endtime=3;
timestepn=endtime*300;
delt=endtime/timestepn;

%savesettings
% save(strcat(pwd,'/',savedir,'/','setings.mat'))

% *** Phase Field Procedure *** (so small and simple piece of code!)
eta=zeros(mboxsize,nboxsize,p)-1;
% making initial structure
x=fix(mboxsize*x);
eta(1:x,:,1)=1;
% ppf is phase variable representing particles
%particledistro(nboxsize,mboxsize,particles_number,radius)
xparticle=nboxsize/2;
yparticle=mboxsize/2;
[ppf]=particledistro(nboxsize,mboxsize,1,grainD,xparticle,yparticle);
% eta(:,5,1)=1;
% eta(:,:,2)=1;
% eta(:,5,2)=0;
% eta=rand(gridn,gridn,p)*0.001;%-0.001;
eta2=zeros(mboxsize,nboxsize,p); %pre-assignment

for tn=1:timestepn
    for i=1:mboxsize
        for j=1:nboxsize
            del2=1/delx^2*(0.5*(eta(indg(i+1,mboxsize),j)-2*eta(i,j)+eta(indg(i-1,mboxsize),j))...
                +0.25*(eta(indg(i+2,mboxsize),j)-2*eta(i,j)+eta(indg(i-2,mboxsize),j)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,nboxsize))-2*eta(i,j)+eta(i,indg(j-1,nboxsize)))...
                +0.25*(eta(i,indg(j+2,nboxsize))-2*eta(i,j)+eta(i,indg(j-2,nboxsize))));
            sumterm=eta(i,j)*sum(eta(i,j).^2)-eta(i,j).^3;
            detadtM=(-alpha.*eta(i,j)+beta.*eta(i,j).^3-kappa.*del2+...
                2*epsilon.*eta(i,j)*ppf(i,j));
            detadt=-L.*(detadtM+2*gamma*sumterm);
            eta2(i,j)=eta(i,j)+delt*detadt;
            %
            %             if eta2(i,j)>1
            %                 eta2(i,j)=1;
            %             end
            %             if eta2(i,j)<-1
            %                 eta2(i,j)=-1;
            %             end
        end
    end

    eta=eta2;

         phi=eta.^2;
    % adding ppf to the phi to make particles positions to 1 inorder to
    %     % make mapping more clear to see it dosen't do anything with the matrix
         phi=phi+ppf;
%         drawg(phi,xparticle,yparticle,tn,eta,ppf)

    eta2(:,:)=boundarycond(eta2,ppf,0);

   

    %     savegrains(eta,ppf,E,xparticle,yparticle,tn,savedir)
end
%
[ME,E]=calculateE(eta,ppf,mboxsize,nboxsize,delx,setings);
 drawE(rot90(rot90(ME)),xparticle,yparticle,tn,eta,ppf)