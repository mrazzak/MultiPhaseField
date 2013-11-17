%function [InterfaceVel]=realtime_particle_friction_solute(param,filenum,savedir)

clear
savedir='/home/cenna/Results/2Dclose/circ_solute/mat/'
filenum=0
accuracy='low';
mkdir(savedir)
% phase field parameters
L=1*1;
m=2;
gamma=1.5*m;
kappa=4;

% solte drag parameters
a=0;
b=4;
% settings structural element
settings.L=L(1);
settings.alpha=m;
settings.beta=m;
settings.gamma=gamma(1);
settings.kappa=kappa(1);
settings.epsilon=nan;
settings.DelG=[0 0];

% geometry settings
p=2;
global nboxsize mboxsize
global delx delt scale
scale=2;
r=fix(40)*scale;
mboxsize=(fix(r)+20)*scale; % y axis in pixels
nboxsize=(fix(r)+20)*scale; % x axis
delx=2/scale;      % length unit per pixel


endtime=50;
timestepn=3000;
delt=endtime/timestepn;
delt=0.01

% *** Phase Field Procedure *** (so small and simple piece of code!)
eta=zeros(mboxsize,nboxsize,p);
% making initial structure
eta(:,:,1)=circlegrain(mboxsize,nboxsize,nboxsize/2,mboxsize/2,r,'circ');
eta(:,:,2)=imcomplement(eta(:,:,1));
eta2=eta;
%savesettings
% save(strcat(pwd,'/',savedir,'/','setings.mat'))

%initialization
for tn=1:50
    tic
    for i=1:mboxsize
        for j=1:nboxsize
            del2=1/delx^2*(0.5*(eta(indg(i+1,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,mboxsize),j,:)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,nboxsize),:)));
            sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
            detadtM=m*(-eta(i,j,:)+eta(i,j,:).^3)+2*gamma*sumterm-kappa.*del2;
            detadt=-L.*(detadtM);
            Mdetadt(i,j)=detadt(1);
            eta2(i,j,:)=eta(i,j,:)+delt*detadt;
        end
    end
    eta=eta2;
    phi=sum(eta(:,:,1:p).^2,3);
%     drawgrains(phi,nan,nan,tn)
    toc
end

se=strel('square',5);
timevec=0;
tn=0;Pf=zeros(nboxsize,mboxsize);
etaVol=etaVolume(eta(:,:,1),delx,nboxsize,mboxsize,accuracy)
while etaVol>10
    %     Mdetadt=zeros(mboxsize,nboxsize);
    tn=tn+1
    [yii,xjj]=find(...
        imerode((phi>0.99999),se)==0);
    
    % Velocity map
    % Mdetadt=(eta2(:,:,1)-eta(:,:,1))/delt;
    [nablaetax,nablaetay]=gradient(eta(:,:,1),delx,delx);
    itr=0;
    error=10;
    while error>1e-5
        Pfpast=Pf;
        Mveloc=Mdetadt./sqrt(nablaetax.^2+nablaetay.^2);
        % remove wrong data from Mveloc
        [maxdetadt,indimaxdetadt]=max(abs(Mdetadt));
        [maxdetadt,indjmaxdetadt]=max(abs(maxdetadt));
        indimaxdetadt=indimaxdetadt(indjmaxdetadt); %indeceis of maximum in detadt where we know maximum of velocity is calculated correctly
        Mveloc(abs(Mveloc)>abs(Mveloc(indimaxdetadt,indjmaxdetadt)))=nan;
        Mveloc=abs(Mveloc);
        Pf=a*Mveloc./(1+b^2*Mveloc.^2);
        Pf(isnan(Pf))=0;
        for ii=1:length(xjj)
            i=yii(ii);j=xjj(ii);
            del2=1/delx^2*(0.5*(eta(indg(i+1,mboxsize),j,:)-2*eta(i,j,:)+eta(indg(i-1,mboxsize),j,:)))...
                +1/delx^2*(0.5*(eta(i,indg(j+1,nboxsize),:)-2*eta(i,j,:)+eta(i,indg(j-1,nboxsize),:)));
            sumterm=eta(i,j,:)*sum(eta(i,j,:).^2)-eta(i,j,:).^3;
            detadtM=m*(-eta(i,j,:)+eta(i,j,:).^3)+2*gamma*sumterm-kappa.*del2;
            detadt=-L*(detadtM+6*eta(i,j,:).*(1-eta(i,j,:))*(Pf(i,j))/2);
            % matrix of the eta.dot
            eta2(i,j,:)=eta(i,j,:)+delt*detadt;
            %         Mdel2(i,j)=eta(i,j,2)*del2(1)^2-eta(i,j,1)*del2(2)^2;
            Mdetadt(i,j)=detadt(1);
        end
        error=sum(sum(abs(Pf-Pfpast)));
        itr=itr+1;
    end
    itr
    %     Mdetadt=(eta2(:,:,1)-eta(:,:,1))/delt;
    eta=eta2;
    phi=sum(eta(:,:,1:p).^2,3);
    timevec(tn+1)=timevec(tn)+delt;
    %%% Visulaizations

    %% simple structure
%   drawgrains(phi,nan,nan,tn)
    %% Volume of each phase field
    etaVol=etaVolume(eta(:,:,1),delx,nboxsize,mboxsize,accuracy);
    MetaVol(:,tn)=etaVol;
end
%

%% calculating average speed
% w=quad(@(x) interp1(delx*[1:nboxsize], eta(mboxsize,:,1),x,'spline'),0,delx*nboxsize);
% pp=polyfit(timevec(2:end),MetaVol(1,:),1);
% InterfaceVel=pp(1)/w
% 
%save evrything
% save([savedir num2str(filenum) '.mat'])

%%
mobility=3/2*L(1)*sqrt(2*kappa(1)/m);
intenergy=1/3*sqrt(2*m*kappa);
 time=timevec;time(1)=[];
    vol=MetaVol;
    radius=sqrt(vol/pi);
    n=1;steps=[1];
%     plot(time,radius)
    spline1 = spaps(time(1:steps(n):end),radius(1:steps(n):end),0.000001,3);
    Mvel=fnval(fnder(spline1,1),time(1:steps(n):end));
% xlabel('Time')
% ylabel('radius of circular grain')
% figure
Mvel=-Mvel;
Mcurvature=1./radius(1:steps(n):end);
plot(Mcurvature,Mvel/mobility/intenergy)
xlabel('\Delta G/ \sigma_{gb}');
ylabel('V / (M \sigma_{gb})');
grid on

axis([0 0.5 0 0.5]);box on
hold on
plot([0 0.5],[0 0.5],'r')
