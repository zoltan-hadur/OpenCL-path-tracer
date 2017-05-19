% function bump_map
%     function linear = sRGB2linear(x)
%         if x<0.04045
%             linear = x/12.92;
%         else
%             linear = power((x+0.055)/1.055, 2.4);
%         end
%     end
% 
%     function sRGB = linear2sRGB(x)
%         if x<=0.0031308
%             sRGB = x*12.92;
%         else
%             sRGB = power(x, 1.0/2.4)*1.055 - 0.055;
%         end
%     end
% 
%     I = imread('stone_3_2k.bmp','bmp');
%     I_double = im2double(I);
% 
% %     R = arrayfun(@sRGB2linear,I_double(:,:,3));
% %     G = arrayfun(@sRGB2linear,I_double(:,:,2));
% %     B = arrayfun(@sRGB2linear,I_double(:,:,1));
% %     I_gray = arrayfun(@linear2sRGB, 0.2126*R + 0.7152*G + 0.0722*B);
% %     figure(3);
% %     imshow(I_gray);
% %     
% %     figure(4);
% %     imshow(rgb2gray(I_double));
% 
%     I_gray = rgb2gray(I_double);
%     figure(1);
%     imshow(I_gray);
%     
%     %right-left component of normals
%     for x = 1:size(I_gray, 2)
%         norm(:, x, 1) = I_gray(:, mod(x+1-1,size(I_gray, 2))+1) - I_gray(:, mod(x-1-1,size(I_gray, 2))+1);
%     end
%     %up-down component of normals
%     for y = 1:size(I_gray, 1)
%         norm(y, :, 2) = I_gray(mod(y+1-1,size(I_gray, 1))+1, :) - I_gray(mod(y-1-1,size(I_gray, 1))+1, :);
%     end
%     norm(:, :, 3) = 1;
%     
% %     norm_1 = norm;
% %     norm_1(:, :, 1) = norm_1(:, :, 1)/2.0 + 0.5;
% %     norm_1(:, :, 2) = norm_1(:, :, 2)/2.0 + 0.5;
% %     
% %     norm_2 = norm;
% %     norm_2(:, :, 1) = tanh(norm_2(:, :, 1))/2.0 + 0.5;
% %     norm_2(:, :, 2) = tanh(norm_2(:, :, 2))/2.0 + 0.5;
% %     
% %     norm_3 = norm;
% %     norm_3(:, :, 1) = tanh(2*norm_3(:, :, 1))/2.0 + 0.5;
% %     norm_3(:, :, 2) = tanh(2*norm_3(:, :, 2))/2.0 + 0.5;
%     
%     norm_4 = norm;
%     norm_4(:, :, 1) = tanh(3*norm_4(:, :, 1))/2.0 + 0.5;
%     norm_4(:, :, 2) = tanh(3*norm_4(:, :, 2))/2.0 + 0.5;
%     
%     norm_5 = norm;
%     norm_5(:, :, 1) = tanh(3*norm_5(:, :, 1)).*exp(-7*abs(norm_5(:, :, 1)))/2.0 + 0.5;
%     norm_5(:, :, 2) = tanh(3*norm_5(:, :, 2)).*exp(-7*abs(norm_5(:, :, 2)))/2.0 + 0.5;
% 
% %     figure(2);
% %     imshow(norm_1);
% %     
% %     figure(3);
% %     imshow(norm_2);
% %     
% %     figure(4);
% %     imshow(norm_3);
%     
%     figure(5);
%     imshow(norm_4);
%     
%     figure(6);
%     imshow(norm_5);
% end

% clear;
x = 0:1:360;
t = [1*sind(x); 2*sind(x); 3*sind(x)];

% net = fitnet([3]);
% 
% [net,tr] = train(net,x,t);


input = -360:360;
target = [sind(input); 2*sind(input); 3*sind(input)];
% target = [sind(input)];
output = net(mod(input,360));
error = gsubtract(output,target);
performance = perform(net,target,output)
plot(input,output);






