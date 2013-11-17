clear
% phase field parameters
L=1;
m=2;
gamma=1.5;
kappa=4;
a=0;
b=1000;
% figure
MGi=linspace(0.1,0.1,40);
for n=1:length(MGi)
    G=[-MGi(n) MGi(n)];
    % geometry settings
    p=2;
    delx=0.5;      % length unit per pixel
    nboxsize=100/delx; % x axis in pixels

    delt=0.01;
    timesteps=30/delt;
    x1=20;
    eta=zeros(2,nboxsize);
    Mdetadtpast=zeros(2,nboxsize);
    xi=[0:nboxsize-1]*delx;
    eta(1,:)=0.5*(1+tanh(sqrt(m/2/kappa)*(x1-xi)));
    eta(2,:)=0.5*(1-tanh(sqrt(m/2/kappa)*(x1-xi)));

    syms v;
    Pfsym=a*v/(1+b*v^2);
    mobility=3/2*L*sqrt(2*kappa/m);
    e1=mobility*(G(2)-Pfsym)-v;
    e1sol=solve(e1);
    vel=eval(e1sol(1));
    %           vel=0.1
    detadtpast=0;
    eta2=eta;
    for tn=1:timesteps
        for p=1:2
            for j=2:nboxsize-1
                err=10;ittr=0;%hold off
                 while err>1e-6 & ittr<50;
                    %               phi(j)=(-eta(1,j)+eta(2,j)+1)/2;
                    phi(j)=eta(p,j);
                    sumterm=eta(1,j)^2+eta(2,j)^2;
                    sumtermp=eta(p,j)*sumterm-eta(p,j).^3;
                    del2=1/delx^2*(eta(p,j+1)+eta(p,j-1)-2*eta(p,j));
                    prof(p,j)=m*(-eta(p,j)+eta(p,j)^3+2*gamma*sumtermp)-kappa.*del2;
                    grad=0.5*(1-(2*eta(p,j)-1).^2)*sqrt(m/2/kappa);
                    velj=Mdetadtpast(p,j)/grad;
                    if isnan(velj)==true | isinf(velj)==true
                        velj=0;
                    end
                    %                 Pf(j)=abs(a*vel/(1+b*vel^2));
                    Pfj=(a*velj/(1+b*velj^2));
                    if abs(Pfj)>abs(G(p))
                        Pfj=-G(p);
                    end
                    detadt=-L.*(prof(p,j)+3*phi(j)*(1-phi(j))*(G(p)+Pfj));
                    err=abs(Mdetadtpast(p,j)-detadt);
                    Mdetadtpast(p,j)=0.5*detadt+0.5*Mdetadtpast(p,j);
                    eta2(p,j)=eta(p,j)+delt*detadt;
                    ittr=ittr+1;
%                      plot(ittr,detadt,'o'); hold on
                 end
                %                 grad=((eta2(p,j+1)-eta2(p,j-1))/2/delx);

                % grad(p,j)=abs((eta(p,j+1)-eta(p,j-1))/2/delx);
            end
        end
        grad2=0.5*(1-(2*eta(1,:)-1).^2)*sqrt(m/2/kappa);
        velj1=Mdetadtpast(1,:)./grad2(1:end);
%         velj2=Mdetadtpast(1,:)./(grad(1,:));
        eta=eta2;
        %
        subplot(2,1,1)
        plot(eta(1,:),'.-')
        hold on
        plot(eta(2,:),'r.-')
        title(num2str(tn));ylabel('\eta')
        hold off
        subplot(2,1,2)
%         plot(grad(1,:),'.-'); hold on
%         plot(grad2,'r.-'); hold off
        plot(abs(velj1),'.-'); hold on; plot([0 nboxsize],[vel vel],'k')
        hold off;axis([0 nboxsize vel-0.5*vel vel+0.5*vel]); ylabel('Local interface Velocity'); xlabel('Position')
        pause(0.01)
    end
    % figure
    % plot(xi,eta(1,:))
    % hold on
    % plot(xi,eta(2,:),'r')
    % title(num2str(tn))
    % hold off
    % pause(0.01)
    ind=find(eta(1,:)>0.1 & eta(1,:)<0.9);
    pos=interp1(eta(1,ind),xi(ind),0.5);
    velcalc=(pos-x1)/(timesteps*delt);
    % syms v;
    % Pf=a*v/(a+b*v^2);
    % delG=G(2);
    % mobility=3/2*L*sqrt(2*kappa/m);
    % e1=mobility*(delG-Pf)-v;
    % e1sol=solve(e1);
    % analytical_vel=eval(e1sol(1));
    % Calculation_Error=(vel-analytical_vel)/analytical_vel*100;
    % out=Calculation_Error
    Mvel(n)=velcalc;
end

% delG vs V plots
mobility=3/2*L*sqrt(2*kappa/m);
intenergy=1/3*sqrt(2*m*kappa);
figure
plot(MGi/intenergy,Mvel/intenergy/mobility,'o')
hold on
axis([0 0.1 0 0.1]);box on
hold on
plot([0 0.5],[0 0.5],'r')

% return
%% analytical

delG=linspace(0,0.2,50);
syms v;
Pfsym=a*v/(1+b*v^2);
ni=0;
for dG=delG
    ni=ni+1;
    e1=mobility*(dG-Pfsym)-v;
    e1sol=solve(e1);
    vel_sol(ni)=eval(e1sol(1));
end
hold on
plot(delG/intenergy,vel_sol/mobility/intenergy,'k')
xlabel('\Delta G/ \sigma_{gb}');
ylabel('V / (M \sigma_{gb})');
